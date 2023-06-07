Для запуска приложения необходимо:
	- Наличие установленной библиотеки boost (в частности ее компонента boost/asio версии не нижу 1.72.0)
	- Наличие установленного клиента PostgreSQL (вместе в библиотекой libpq для взаимодействия с БД)
	- Наличие автоматического сборщика CMake версии не ниже 3.22.1
	- Наличие установленных компонентов C++ (не ниже 17 стандарта)
	
Прежде чем приступать к постройке проекта, необходимо сформировать базу данных. Для этого см. файл sql_sripts.txt

Чтобы построить проект необходимо перейти в директорию проекта 'Testovoe' и из терминала выполнить следующую команду:
	cdmod +x BUILD.sh

После чего запустить скрипт BUILD.sh, используя команду терминала
	./BUILD.sh

После успешной сборки проекта можно перейти к запуску. Для запуска программы в автоматическом режиме (Запускается сервер и приложение для нагрузки) выполните скрипт LAUCNH.sh, используя
	./LAUNCH.sh  
	
Если необходимо протестировать приложение в ручном режиме, тогда сперва перейдите в папку server. Откройте терминал и запустите скрипт launch.sh
	./launch.sh
	
После этого перейдите в папку client и в новом терминале запустите скрипт launch.sh
	./launch.sh
	
При успешном запуске клиентское приложение попросит вас ввести логин и пароль. Доступные комбинации:
	user1 1234
	user2 1234
	user3 1234
	user4 1234
	user5 1234
	user6 1234
	user7 1234
	user8 1234
	user9 1234

Остальные варианты не пройдут верификацию на сервере.

