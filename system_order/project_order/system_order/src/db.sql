create database if not exists system_order;

use system_order;

create table if not exists tb_dish(
  id int primary key auto_increment,
  name varchar(32) unique not null,
  price int not null,
  ctime datetime
);

create table if not exists tb_order(
  id int primary key auto_increment,
  dishes varchar(255) comment '[1,2]',
  status int comment'0-未完成 1-完成',
  mtime datetime

);

insert tb_dish values(null,"梅菜扣肉",4500,now()),
                     (null,"土豆盐煎肉",3200,now());
insert tb_order values(null,"[1,2]",0,now()),
                      (null,"[1]",0,now());



