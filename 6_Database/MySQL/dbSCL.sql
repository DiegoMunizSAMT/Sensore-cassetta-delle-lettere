CREATE DATABASE dbSCL; #Sensore Cassetta delle Lettere
USE dbSCL;

CREATE TABLE User
(
email VARCHAR(320) PRIMARY KEY,
username TEXT,
password VARCHAR(100)
);

CREATE TABLE Notification
(
id INT PRIMARY KEY auto_increment,
comment TEXT
);

CREATE TABLE Sensor
(
id INT PRIMARY KEY auto_increment,
type TEXT,
value BOOLEAN,
arrival_time DATETIME,
notif_id INT,
FOREIGN KEY(notif_id) REFERENCES Notification(id)
);

CREATE TABLE Device
(
id INT PRIMARY KEY auto_increment,
model TEXT
);

CREATE TABLE DeviceHasSensor
(
dev_id INT,
snsr_id INT,
FOREIGN KEY(dev_id) REFERENCES Device(id),
FOREIGN KEY(snsr_id) REFERENCES Sensor(id)
);

CREATE TABLE UserHasDevice
(
user_email VARCHAR(320),
dev_id INT,
FOREIGN KEY(user_email) REFERENCES User(email),
FOREIGN KEY(dev_id) REFERENCES Device(id)
);

CREATE TABLE NotificationNotifiesUser
(
notif_id INT,
user_email VARCHAR(320),
FOREIGN KEY(notif_id) REFERENCES Notification(id),
FOREIGN KEY(user_email) REFERENCES User(email)
);