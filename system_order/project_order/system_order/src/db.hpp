#include<iostream>
#include<jsoncpp/json/json.h>
#include<mysql/mysql.h>
#include<mutex> 
#include<string>
namespace sys_order
{
#define MYSQL_SRV "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD ""
#define MYSQL_DBNAME "system_order"

  static MYSQL* MysqlInit()
  {
    MYSQL* mysql=NULL;
    //初始化句柄
    mysql=mysql_init(NULL);
    if(mysql==NULL)
    {
      std::cout<<"mysql init error\n";
      return NULL;
    }
    //连接mysql服务器
    if(mysql_real_connect(mysql,MYSQL_SRV,MYSQL_USER,MYSQL_PSWD,MYSQL_DBNAME,0,NULL,0)==NULL)
    {
      std::cout<<mysql_error(mysql)<<std::endl;
      return NULL;
    }
    //设置字符集
    if(mysql_set_character_set(mysql,"utf8")!=0)
    {
      std::cout<<mysql_error(mysql)<<std::endl;
      return NULL;
    }
    //选择数据库
    //在连接mysql服务器中已经指定数据库，不再操作。
    return mysql;
  }

  static void MysqlRelease(MYSQL* mysql)
  {
    if(mysql!=NULL)
    {
      mysql_close(mysql);
    }
    return;
  }

  static bool MysqlQuery(MYSQL* mysql,const std::string& sqlstr)
  {
    if(mysql_query(mysql,sqlstr.c_str())!=0)
    {
      std::cout<<mysql_error(mysql)<<std::endl;
      return false;
    }
    return true;
  }

  class TableDish
  {
    private:
      MYSQL* _mysql;
      std::mutex _mutex;
    public:
      TableDish()
      {
        //完成初始化
        _mysql=MysqlInit();
        if(_mysql==NULL)
        {
          exit(-1);
        }
      }
      ~TableDish()
      {
        //释放资源
        MysqlRelease(_mysql);
        _mysql=NULL;
      }
      bool Insert(const Json::Value& dish )
      {
#define DISH_INSERT "insert tb_dish values (null,'%s',%d,now());"
        char sql_str[4096]={0};
        sprintf(sql_str,DISH_INSERT,dish["name"].asCString(),dish["price"].asInt());
        return MysqlQuery(_mysql,sql_str);
      }

      bool Delete(int dish_id)
      {
#define DISH_DELETE "delete from tb_dish where id=%d;"
        char sql_str[4096]={0};
        sprintf(sql_str,DISH_DELETE,dish_id);
        return MysqlQuery(_mysql,sql_str);
      }
      bool Update(const Json::Value& dish)
      {
#define DISH_UPDATE "update tb_dish set name='%s' ,price=%d where id=%d;"
        char sql_str[4096]={0};
        sprintf(sql_str,DISH_UPDATE,dish["name"].asCString(),dish["price"].asInt(),dish["id"].asInt());
        return MysqlQuery(_mysql,sql_str);
      }
      bool SelectAll(Json::Value* dishes)
      {
#define DISH_SELECTALL "select *from tb_dish;"
        //执行查询语句
        //这两步骤需要加锁保护
        _mutex.lock();
        bool ret = MysqlQuery(_mysql,DISH_SELECTALL);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        //将查询结果保存到本地
        MYSQL_RES* res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store failed\n";
          return false;
        }
        //获取行数
        int num =mysql_num_rows(res);
        //遍历结果集
        for(int i = 0;i<num;i++)
        {
          MYSQL_ROW row =mysql_fetch_row(res);
          Json::Value dish;
          dish["id"]=std::stoi(row[0]);
          dish["name"]=row[1];
          dish["price"]=std::stoi(row[2]);
          dish["ctime"]=row[3];
          dishes->append(dish);
        }
        //释放结果集
        mysql_free_result(res);
        return true;
      }

      bool SelectOne(int dish_id,Json::Value* dish)
      {

#define DISH_SELECTONE "select *from tb_dish where id = %d;"
        char sql_str[4096]={0};
        sprintf(sql_str,DISH_SELECTONE,dish_id);
        //执行查询语句
        //这两步骤需要加锁保护
        _mutex.lock();
        bool ret = MysqlQuery(_mysql,sql_str);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        //将查询结果保存到本地
        MYSQL_RES* res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store failed\n";
          return false;
        }
        //获取行数
        int num = mysql_num_rows(res);
        if(num!=1)
        {
          std::cout<<"result error\n";
          mysql_free_result(res);
          return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        (*dish)["id"]=dish_id;
        (*dish)["name"]=row[1];
        (*dish)["price"]=std::stoi(row[2]);
        (*dish)["ctime"]=row[3];
        mysql_free_result(res);
        return true;
      }
  };


  class TableOrder
  {
    private:
      MYSQL* _mysql;
      std::mutex _mutex;
    public:
      TableOrder()
      {
        //完成初始化
        _mysql=MysqlInit();
        if(_mysql==NULL)
        {
          exit(-1);
        }
      }
      ~TableOrder()
      {
        //释放资源
        MysqlRelease(_mysql);
        _mysql=NULL;
      }
      bool Insert(const Json::Value& order )
      {
#define ORDER_INSERT "insert tb_order values (null,'%s',0,now());"
        char sql_str[4096]={0};
        Json::FastWriter writer;
        std::string dishes=writer.write(order["dishes"]);
        //将\n修改成\0保证tb_order数据表中dishes不换行
        dishes[dishes.size()-1]='\0';
        sprintf(sql_str,ORDER_INSERT,dishes.c_str());
        return MysqlQuery(_mysql,sql_str);
      }

      bool Delete(int order_id)
      {
#define ORDER_DELETE "delete from tb_order where id=%d;"
        char sql_str[4096]={0};
        sprintf(sql_str,ORDER_DELETE,order_id);
        return MysqlQuery(_mysql,sql_str);
      }
      bool Update(const Json::Value& order)
      {
#define ORDER_UPDATE "update tb_order set dishes='%s' ,status=%d where id=%d;"
        char sql_str[4096]={0};
        Json::FastWriter writer;
        std::string dishes=writer.write(order["dishes"]);
        sprintf(sql_str,ORDER_UPDATE,dishes.c_str(),order["status"].asInt(),order["id"].asInt());
        return MysqlQuery(_mysql,sql_str);
      }
      bool SelectAll(Json::Value* orders)
      {
#define ORDER_SELECTALL "select *from tb_order;"
        //执行查询语句
        //这两步骤需要加锁保护
        _mutex.lock();
        bool ret = MysqlQuery(_mysql,ORDER_SELECTALL);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        //将查询结果保存到本地
        MYSQL_RES* res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store failed\n";
          return false;
        }
        //获取行数
        int num =mysql_num_rows(res);
        //遍历结果集
        for(int i = 0;i<num;i++)
        {
          MYSQL_ROW row =mysql_fetch_row(res);
          Json::Value order;
          order["id"]=std::stoi(row[0]);
          //row[1]是json格式字符串,反序列化保存在dishes对象中
          Json::Reader reader;
          Json::Value dishes;
          if(reader.parse(row[1],dishes))
          {
            order["dishes"]=dishes;
          }
          order["status"]=std::stoi(row[2]);
          order["mtime"]=row[3];
          orders->append(order);
        }
        //释放结果集
        mysql_free_result(res);
        return true;
      }

      bool SelectOne(int order_id,Json::Value* order)
      {

#define ORDER_SELECTONE "select *from tb_order where id = %d;"
        char sql_str[4096]={0};
        sprintf(sql_str,ORDER_SELECTONE,order_id);
        //执行查询语句
        //这两步骤需要加锁保护
        _mutex.lock();
        bool ret = MysqlQuery(_mysql,sql_str);
        if(ret==false)
        {
          _mutex.unlock();
          return false;
        }
        //将查询结果保存到本地
        MYSQL_RES* res=mysql_store_result(_mysql);
        _mutex.unlock();
        if(res==NULL)
        {
          std::cout<<"store failed\n";
          return false;
        }
        //获取行数
        int num = mysql_num_rows(res);
        if(num!=1)
        {
          std::cout<<"result error\n";
          mysql_free_result(res);
          return false;
        }
        MYSQL_ROW row = mysql_fetch_row(res);
        (*order)["id"]=order_id; 
        Json::Reader reader;
        Json::Value dishes;
        if(reader.parse(row[1],dishes))
        {
           (*order)["dishes"]=dishes;
        }
        (*order)["status"]=std::stoi(row[2]);
        (*order)["mtime"]=row[3];
        mysql_free_result(res);
        return true;
      }
  };

}
