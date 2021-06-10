# SPO lab 3
##  Library catalog

Программа может выполняться в двух режимах: сервер или клиент.
Режим определяется аргументом командной строки.
Завершение программы-сервера происходит по нажатию ключевой клавиши (например, Q).
При  запуске  в  режиме  клиента  чрез  аргументы  командной 
строки  задаётся  имя пользователя и адрес сервера.

Каждая книга описывается карточкой, содержащей сведения о названии,
авторе, дате издания, аннотации, и наборе тэгов (таких как жанры и т.п.),
количество приписанных к каталогу и количество в наличии.
Каждый подключающийся пользователь может помещать книги в каталог,
брать  на  прочтение  и  возвращать.
Предусмотреть  поиск  сведений  о  книгах  по  различным критериям.
Если описание книги редактируется или изменяется статус наличия,
пользователям должна своевременно показываться актуальная информация
по мере её изменения. Визуально любые  операции  редактирования
осуществляются «in-place»,  то  естьпо  месту  визуализации данных поля.