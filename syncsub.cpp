/* Исходный код программы потребителя (БСКИ), который получает информацию о координате и высоте от программ производителей (НО и РВ) и выводит их в терминал.
      Разработчик: Ляшун Д. С. */

#include "interface.hpp"
#include <unistd.h>
#include <iostream>
#include <string>

int main () {  
    const std::string SUBSCRIBER_NAME = "БСКИ"; // Имя потребителя
    const int32_t SUBSCRIBER_SLEEP_TIME = 10000; // Время ожидания потребителя между получением сообщений (в микросекундах)
    
    zmq::context_t context(1); // Создание контекста (процесса, который управляет работой сокетов в рамках программы)
    
    // Создание сокета-подписчика и связывание его с адресами публикаторов
    zmq::socket_t subscriber (context, ZMQ_SUB);
    subscriber.connect("tcp://localhost:5561");
    subscriber.connect("tcp://localhost:5562");
    subscriber.set(zmq::sockopt::rcvhwm, 0); // Устанавливает неограниченное число хранимых сообщений в очереди получателя
    subscriber.set(zmq::sockopt::subscribe, ""); // Устанавливает подписку - префикс данных сообщений, которые необходимо отбирать

    //  Создание сокета для предварительной связи с публикаторами с целью синхронизации потока данных
    zmq::socket_t sync_coord_gen(context, ZMQ_REQ);
    zmq::socket_t sync_altit_gen(context, ZMQ_REQ);
    
    std::cout << "[ " << SUBSCRIBER_NAME << ", pid = " << getpid() << " ]: Начинаю устанавливать связь с публикаторами." << std::endl;
    sync_coord_gen.connect("tcp://localhost:5563");
    nZMQInterface::send_data(sync_coord_gen, "");
    nZMQInterface::recieve_message(sync_coord_gen);
    
    sync_altit_gen.connect("tcp://localhost:5564");
    nZMQInterface::send_data(sync_altit_gen, "");
    nZMQInterface::recieve_message(sync_altit_gen);
    std::cout << "[ " << SUBSCRIBER_NAME << ", pid = " << getpid() << " ]: Связь с публикаторами успешно установлена." << std::endl;
    
    
    //  Процесс получения данных от публикаторов
    bool b_last_coordinate_data = false, b_last_altitude_data = false;
    while (!b_last_coordinate_data || !b_last_altitude_data) {
        auto res = nZMQInterface::recieve_data(subscriber);
        if (res.has_value()) {
            nZMQInterface::rData data = res.value();
            switch (data.m_e_type) {
                case nZMQInterface::eDataType::ALTITUDE_END:
                    std::cout << "[ " << SUBSCRIBER_NAME << ", pid = " << getpid() << " ]: Получил последние данные о высоте." << std::endl;
                    b_last_altitude_data = true;
                    break;
                case nZMQInterface::eDataType::COORDINATE_END:
                    std::cout << "[ " << SUBSCRIBER_NAME << ", pid = " << getpid() << " ]: Получил последние данные о координате." << std::endl;
                    b_last_coordinate_data = true;
                    break;
                case nZMQInterface::eDataType::ALTITUDE:
                    std::cout << "[ " << SUBSCRIBER_NAME << ", pid = " << getpid() << " ]: Получил высоту H = " << data.m_l_altitude << "." << std::endl;
                    break;
                case nZMQInterface::eDataType::COORDINATE:
                    std::cout << "[ " << SUBSCRIBER_NAME << ", pid = " << getpid() << " ]: Получил координаты X = " << data.m_l_x_coord << ", Y = " << data.m_l_y_coord << "." << std::endl;
                    break;
            }
            usleep(SUBSCRIBER_SLEEP_TIME);
        }
        else { 
            std::cerr << "Произошла ошибка при попытке получения сообщения со стороны потребителя!" << std::endl;
            return -2;
        }
    }
    sleep (1);  // Дать ZeroMQ время чтобы сбросить вывод
    return 0;
}
