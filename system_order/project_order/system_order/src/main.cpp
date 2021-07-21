#include"db.hpp"
#include "httplib.h"

#define WWWROOT "./wwwroot"

using namespace httplib;

sys_order::TableDish *tb_dish=NULL;
sys_order::TableOrder *tb_order=NULL;
void DishInsert(const Request &req,Response &rsp)
{
  //业务处理
  //解析正文 得到菜品信息 插入数据库
  Json::Reader reader;
  Json::Value dish;
  bool ret =  reader.parse(req.body,dish);
  if(ret==false)
  {
    rsp.status=400;
    Json::FastWriter writer;
    Json::Value reason;
    reason["result"]=false;
    reason["reason"]="dish info parse failed!";
    rsp.body=writer.write(reason);
    rsp.set_header("Content-Type","application/json");
    std::cout<<"insert dish parse error\n";
    return;
  }
  ret=tb_dish->Insert(dish);
  if(ret==false)
  {
    rsp.status=500;
    Json::FastWriter writer;
    Json::Value reason;
    reason["result"]=false;
    reason["reason"]="mysql insert failed!";
    rsp.body=writer.write(reason);
    rsp.set_header("Content-Type","application/json");
    return;
  }
  //设置响应信息 
  rsp.status=200;
  return;
}

void DishDelete(const Request &req,Response &rsp)
{
  int dish_id=std::stoi(req.matches[1]);
  bool ret = tb_dish->Delete(dish_id);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"mysql delete dish error\n";
    return;
  }

  return;
}

void DishUpdate(const Request &req,Response &rsp)
{
  int dish_id=std::stoi(req.matches[1]);
  Json::Value dish;
  Json::Reader reader;
  bool ret=reader.parse(req.body,dish);
  if(ret==false)
  {
    rsp.status=400;
    std::cout<<"update parse dish info failed\n";
    return;
  }
  dish["id"]=dish_id;
  ret=tb_dish->Update(dish);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"update mysql update dish error\n";
    return;
  }
  return;
}

void DishGetAll(const Request &req,Response &rsp)
{
  Json::Value dishes;
  bool ret=tb_dish->SelectAll(&dishes);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"select all mysql get dish error\n";
  }
  Json::FastWriter writer;
  rsp.body=writer.write(dishes);
  return;
}
void DishGetOne(const Request &req,Response &rsp)
{
  int dish_id=std::stoi(req.matches[1]);
  Json::Value dish;
  bool ret=tb_dish->SelectOne(dish_id,&dish);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"select one mysql get dish error\n";
  }
  Json::FastWriter writer;
  rsp.body=writer.write(dish);
  return;
}

void OrderInsert(const Request &req,Response &rsp)
{
  Json::Reader reader;
  Json::Value order;
  bool ret =  reader.parse(req.body,order);
  if(ret==false)
  {
    rsp.status=400;
    Json::FastWriter writer;
    Json::Value reason;
    reason["result"]=false;
    reason["reason"]="order info parse failed!";
    rsp.body=writer.write(reason);
    rsp.set_header("Content-Type","application/json");
    std::cout<<"insert order parse error\n";
    return;
  }
  ret=tb_order->Insert(order);
  if(ret==false)
  {
    rsp.status=500;
    Json::FastWriter writer;
    Json::Value reason;
    reason["result"]=false;
    reason["reason"]="mysql insert failed!";
    rsp.body=writer.write(reason);
    rsp.set_header("Content-Type","application/json");
    return;
  }
  //设置响应信息 
  return;
}


void OrderDelete(const Request &req,Response &rsp)
{
  int order_id=std::stoi(req.matches[1]);
  bool ret = tb_order->Delete(order_id);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"mysql delete order error\n";
    return;
  }

  return;
}

void OrderUpdate(const Request &req,Response &rsp)
{
  int order_id=std::stoi(req.matches[1]);
  Json::Value order;
  Json::Reader reader;
  bool ret=reader.parse(req.body,order);
  if(ret==false)
  {
    rsp.status=400;
    std::cout<<"update parse order info failed\n";
    return;
  }
  order["id"]=order_id;
  ret=tb_order->Update(order);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"update mysql update order error\n";
    return;
  }
  return;
}

void OrderGetAll(const Request &req,Response &rsp)
{
  Json::Value orders;
  bool ret=tb_order->SelectAll(&orders);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"select all mysql get order error\n";
  }
  Json::FastWriter writer;
  rsp.body=writer.write(orders);
  return;
}
void OrderGetOne(const Request &req,Response &rsp)
{
  int order_id=std::stoi(req.matches[1]);
  Json::Value order;
  bool ret=tb_order->SelectOne(order_id,&order);
  if(ret==false)
  {
    rsp.status=500;
    std::cout<<"select one mysql get order error\n";
  }
  Json::FastWriter writer;
  rsp.body=writer.write(order);
  return;
}


int main()
{
  tb_dish = new sys_order::TableDish();
  tb_order = new sys_order::TableOrder();
  Server server;
  server.set_base_dir(WWWROOT);
  server.Post("/dish",DishInsert);
  //正则表达式 \d+ 表示匹配一个数字字符一次或者多次
  //R"()"去除特殊字符的特殊含义
  server.Delete(R"(/dish/(\d+))",DishDelete);
  server.Put(R"(/dish/(\d+))",DishUpdate);
  server.Get(R"(/dish)",DishGetAll);
  server.Get(R"(/dish/(\d+))",DishGetOne);
  server.Post("/order",OrderInsert);
  server.Delete(R"(/order/(\d+))",OrderDelete);
  server.Put(R"(/order/(\d+))",OrderUpdate);
  server.Get(R"(/order)",OrderGetAll);
  server.Get(R"(/order/(\d+))",OrderGetOne);
  server.listen("0.0.0.0",8888);
  return 0;
}
