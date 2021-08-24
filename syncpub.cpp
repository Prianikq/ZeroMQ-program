/* Исходный код программы производителя координат или высоты (в зависимости от указанного аргумента в командной строке), 
   которые затем посылаются потребителю - БСКИ.
        Разработчик: Ляшун Д. С. */
#include "interface.hpp"
#include <iostream>
#include <string>
#include <unistd.h>

int32_t main (int32_t p_l_argc, const char *p_p_argv[]) {
    const int32_t NUMBER_COMMAND_LINE_ARGUMENTS = 5; // Требуемое число аргументов в командной строке для запуска программы
    const std::string COORDINATE_GENERATOR_NAME = "НО"; // Имя производителя координат
    const std::string ALTITUDE_GENERATOR_NAME = "РВ"; // Имя производителя высоты
    const int32_t MAXIMUM_COORDINATE = 100; // Предельное значение координат (при его достижении прекращается отправка)
    const int32_t MAXIMUM_ALTITUDE = 100; // Предельное значение высоты
    //const int32_t COORDINATE_GENERATOR_WAIT_TIME = 40000; // Время ожидания производителя координат между отправками данных (в микросекундах)
    //const int32_t ALTITUDE_GENERATOR_WAIT_TIME = 200000; // Время ожидания производителя высоты между отправками данных (в микросекундах)
    const short HANDSHAKING_PORT = 5565; // Номер порта, по которому происходит "рукопожатие" между производителями перед началом публикации данных
    
    std::string s_publisher_type; // Хранит тип производителя, который задается через аргументы командной строки
    short n_sync_port; // Хранит номер порта, который используется данным производителем для установления связи-синхронизации с подписчиком
    short n_publ_port; // Хранит номер порта, который используется данным производителем для отправки данных подписчику
    int32_t l_wait_time; // Время ожидания производителя между отправками данных (в микросекундах)
    
    if (p_l_argc != NUMBER_COMMAND_LINE_ARGUMENTS) {
        std::cerr << "Ошибка! Произошел запуск программы производителя с некорректными аргументами командной строки!" << std::endl;
        return -1;
    }
    else {
        s_publisher_type = std::string(p_p_argv[1]);
        n_sync_port = atoi(p_p_argv[2]);
        n_publ_port = atoi(p_p_argv[3]);
        l_wait_time = atoi(p_p_argv[4]);
        if (!(s_publisher_type == COORDINATE_GENERATOR_NAME || s_publisher_type == ALTITUDE_GENERATOR_NAME)) {
            std::cerr << "Ошибка! Указан неверный тип производителя в аргументе командной строки!" << std::endl;
            return -2;
        }
    }
    
    // Создание сокета-публикатора и установление связи по адресу
    zmq::context_t context(1); // Создание контекста (процесса, который управляет работой сокетов в рамках программы)
    zmq::socket_t publisher (context, ZMQ_PUB);
    publisher.set(zmq::sockopt::sndhwm, 0); // Устанавливает неограниченное число хранимых сообщений в очереди получателя
    publisher.bind("tcp://*:" + std::to_string(n_publ_port));

    //  Установление связи с БСКИ
    zmq::socket_t syncservice (context, ZMQ_REP);
    syncservice.bind("tcp://*:" + std::to_string(n_sync_port));
    nZMQInterface::recieve_message(syncservice);
    nZMQInterface::send_data(syncservice, ""); // Отправка пустого сообщения БСКИ как факт подтверждения получения от него предыдущего сообщения
    
    // Установление связи с другим публикатором для гарантии того, что БСКИ успешно связался со всеми публикаторами и готов принимать их сообщения
    zmq::socket_t handshaking(context, (s_publisher_type == "НО"? ZMQ_REQ : ZMQ_REP));
    if (s_publisher_type == COORDINATE_GENERATOR_NAME) {
        std::cout << "[ " << s_publisher_type << ", pid = " << getpid() << " ]: Начинаю связываться с " << ALTITUDE_GENERATOR_NAME << "." << std::endl;
        handshaking.connect("tcp://localhost:" + std::to_string(HANDSHAKING_PORT));
        nZMQInterface::send_data(handshaking, "");
        nZMQInterface::recieve_message(handshaking);
    }
    else {
        handshaking.bind("tcp://*:" + std::to_string(HANDSHAKING_PORT));
        nZMQInterface::recieve_message(handshaking);
        nZMQInterface::send_data(handshaking, "");
        std::cout << "[ " << s_publisher_type << ", pid = " << getpid() << " ]: Связь с " << COORDINATE_GENERATOR_NAME << " успешно установлена." << std::endl;
        usleep(l_wait_time);
    }
    
    //  Процесс отправки сообщений
    int32_t l_x_coord = 0, l_y_coord = 0, l_altitude = 0;
    while (l_x_coord <= MAXIMUM_COORDINATE && l_altitude <= MAXIMUM_ALTITUDE) {
        if (s_publisher_type == COORDINATE_GENERATOR_NAME) {
            std::cout << "[ " << s_publisher_type << ", pid = " << getpid() << " ]: Отправляю координаты X = " << l_x_coord << " Y = " << l_y_coord << ". " << std::endl;	
            if (nZMQInterface::send_data(publisher, nZMQInterface::rData(nZMQInterface::eDataType::COORDINATE, l_x_coord, l_y_coord)) == -1) {
                std::cerr << "Произошла ошибка при попытке отправки сообщения!" << std::endl;
                return -3;
            }
            ++l_x_coord;
            ++l_y_coord;
            usleep(l_wait_time);
        }
        else {
            std::cout << "[ " << s_publisher_type << ", pid = " << getpid() << " ]: Отправляю высоту H = " << l_altitude << ". " << std::endl;
            if (nZMQInterface::send_data(publisher, nZMQInterface::rData(nZMQInterface::eDataType::ALTITUDE, l_altitude)) == -1) {
                std::cerr << "Произошла ошибка при попытке отправки сообщения!" << std::endl;
                return -3;
            }
            ++l_altitude;
            usleep(l_wait_time);
        }
    }
	
    if (nZMQInterface::send_data(publisher, nZMQInterface::rData(s_publisher_type == COORDINATE_GENERATOR_NAME? nZMQInterface::eDataType::COORDINATE_END : nZMQInterface::eDataType::ALTITUDE_END)) == -1) {
        std::cerr << "Произошла ошибка при попытке отправки завершающего сообщения!" << std::endl;
        return -4;
    }
    std::cout << "[ " << s_publisher_type << ", pid = " << getpid() << " ]: Отправил все сообщения, завершаю работу. " << std::endl;	
    sleep (1);  // Дать ZeroMQ время чтобы сбросить вывод
    return 0;
}
