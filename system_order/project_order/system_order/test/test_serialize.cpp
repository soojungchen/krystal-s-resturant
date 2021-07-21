#include <jsoncpp/json/json.h>
#include<iostream>
std::string serialize()
{
  const  char* name="郑秀晶";
  int age = 26;
  int date[3]={1994,10,24};
  Json::Value val;
  val["姓名"]=name;
  val["年龄"]=age;
  for(int i = 0;i<3;i++)
  {
    val["生日"].append(date[i]);
  }
  Json::StyledWriter sw;
  std::string ret = sw.write(val);
  return ret;

}
void ReadJson(std::string& str)
{
  Json::Value val;
  Json:: Reader rd;
  if(rd.parse(str,val))
  {
    std::string out1 = val["姓名"].asString();
    std::cout<<out1<<std::endl;
    int out2=val["年龄"].asInt();
    std::cout<<out2<<std::endl;
    for(int i = 0;i<3;i++)
    {
      int out3=val["生日"][i].asInt();
      std::cout<<out3<<" ";
    }
    std::cout<<std::endl;
  }
}
int main()
{
  std:: string str=serialize();
  std::cout<<str<<std::endl;
  std::cout<<"--------------------------------------------------------"<<std::endl;
  ReadJson(str);
  return 0;
}
