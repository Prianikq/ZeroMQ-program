/* Исходный код программы производителя координат или высоты (в зависимости от указанного аргумента в командной строке), 
   которые затем посылаются потребителю - БСКИ.
        Разработчик: Ляшун Д. С. */
#include "interface.hpp"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

int32_t main (int32_t p_l_argc, const char *p_p_argv[]) {
    using namespace std::chrono_literals;
    const int32_t NUMBER_COMMAND_LINE_ARGUMENTS = 4; // Требуемое число аргументов в командной строке для запуска программы
    const std::string COORDINATE_GENERATOR_NAME = "НО"; // Имя производителя координат
    const std::string ALTITUDE_GENERATOR_NAME = "РВ"; // Имя производителя высоты
    const int32_t MAXIMUM_COORDINATE = 100; // Предельное значение координат (при его достижении прекращается отправка)
    const int32_t MAXIMUM_ALTITUDE = 100; // Предельное значение высоты
    const short SUBS_SYNC_PORT = 5565; // Номер порта, по которому производитель получает приветствие от потребителей (подтверждение их создания)
    const short PUBS_SYNC_PORT = 5566;
    const short SUBSCRIBERS_COUNT = 3; // Общее число потребителей
    const short PUBLISHERS_COUNT = 2; // Общее число производителей
    
    std::string s_publisher_type; // Хранит тип производителя, который задается через аргументы командной строки
    short n_publ_port; // Хранит номер порта, который используется данным производителем для отправки данных подписчику
    int32_t l_wait_time; // Время ожидания производителя между отправками данных (в микросекундах)
    
    if (p_l_argc != NUMBER_COMMAND_LINE_ARGUMENTS) {
        std::cerr << "Ошибка! Произошел запуск программы производителя с некорректными аргументами командной строки!" << std::endl;
        return -1;
    }
    else {
        s_publisher_type = std::string(p_p_argv[1]);
        n_publ_port = atoi(p_p_argv[2]);
        l_wait_time = atoi(p_p_argv[3]);
        if (!(s_publisher_type == COORDINATE_GENERATOR_NAME || s_publisher_type == ALTITUDE_GENERATOR_NAME)) {
            std::cerr << "Ошибка! Указан неверный тип производителя в аргументе командной строки!" << std::endl;
            return -2;
        }
    }
    
    // Создание сокета-публикатора и установление связи по адресу
    zmq::context_t context(1); // Создание контекста (процесса, который управляет работой сокетов в рамках программы)
    zmq::socket_t publisher (context, ZMQ_PUB);
    publisher.set(zmq::sockopt::conflate, true); // Устананавливает возможность хранения только одного сообщения в очереди отправителя
    publisher.bind("tcp://*:" + std::to_string(n_publ_port));

    std::cout << "[ " << s_publisher_type << " ]: Начинаю устанавливать связь с потребителями. " << std::endl;
    if (s_publisher_type == COORDINATE_GENERATOR_NAME) {
        /* Получение приветствий от потребителей */
        zmq::socket_t subs_check(context, ZMQ_REP);
        subs_check.bind("tcp://*:" + std::to_string(SUBS_SYNC_PORT));
        short n_count = 0;
        while (n_count != SUBSCRIBERS_COUNT) {
            nZMQInterface::recieve_message(subs_check);
            nZMQInterface::send_data(subs_check, ""); // Отправка пустого сообщения БСКИ как факт подтверждения получения от него предыдущего сообщения
            ++n_count;
        }
                
        /* Рассылка сообщений остальным производителям как факт того, что все потребители готовы получать сообщения */
        zmq::socket_t pubs_check(context, ZMQ_REP);
        pubs_check.bind("tcp://*:" + std::to_string(PUBS_SYNC_PORT));
        n_count = 1;
        while (n_count != PUBLISHERS_COUNT) {
            nZMQInterface::recieve_message(pubs_check);
            nZMQInterface::send_data(pubs_check, "");
            ++n_count;
        }
    }
    else {
        zmq::socket_t pubs_check(context, ZMQ_REQ);
        pubs_check.connect("tcp://localhost:" + std::to_string(PUBS_SYNC_PORT));
        nZMQInterface::send_data(pubs_check, "");
        nZMQInterface::recieve_message(pubs_check);
    }
    std::cout << "[ " << s_publisher_type << " ]: Связь с потребителями успешно установлена. " << std::endl;
    
    //  Процесс отправки сообщений
    int32_t l_x_coord = 0, l_y_coord = 0, l_altitude = 0;
    while (l_x_coord <= MAXIMUM_COORDINATE && l_altitude <= MAXIMUM_ALTITUDE) {
        if (s_publisher_type == COORDINATE_GENERATOR_NAME) {
            std::cout << "[ " << s_publisher_type << " ]: Отправляю координаты X = " << l_x_coord << " Y = " << l_y_coord << ". " << std::endl;	
            if (nZMQInterface::send_data(publisher, nZMQInterface::rData(nZMQInterface::eDataType::COORDINATE, l_x_coord, l_y_coord)) == -1) {
                std::cerr << "Произошла ошибка при попытке отправки сообщения!" << std::endl;
                return -3;
            }
            ++l_x_coord;
            ++l_y_coord;
            std::this_thread::sleep_for(std::chrono::microseconds(l_wait_time));
        }
        else {
            std::cout << "[ " << s_publisher_type << " ]: Отправляю высоту H = " << l_altitude << ". " << std::endl;
            if (nZMQInterface::send_data(publisher, nZMQInterface::rData(nZMQInterface::eDataType::ALTITUDE, l_altitude)) == -1) {
                std::cerr << "Произошла ошибка при попытке отправки сообщения!" << std::endl;
                return -3;
            }
            ++l_altitude;
            std::this_thread::sleep_for(std::chrono::microseconds(l_wait_time));
        }
    }
	
    if (nZMQInterface::send_data(publisher, nZMQInterface::rData(s_publisher_type == COORDINATE_GENERATOR_NAME? nZMQInterface::eDataType::COORDINATE_END : nZMQInterface::eDataType::ALTITUDE_END)) == -1) {
        std::cerr << "Произошла ошибка при попытке отправки завершающего сообщения!" << std::endl;
        return -4;
    }
    std::cout << "[ " << s_publisher_type << " ]: Отправил все сообщения, завершаю работу. " << std::endl;	
    std::this_thread::sleep_for(1s);  // Дать ZeroMQ время чтобы сбросить вывод
    return 0;
}
