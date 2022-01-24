/*
   Serial
   串口通讯实验
*/
int incomedate = 0;
void setup() {

  Serial.begin(9600); //设置串口波特率9600

}

void loop() {
  if (Serial.available() > 0)//串口接收到数据
  {
    incomedate = Serial.read();//获取串口接收到的数据
    Serial.println("Data coming");
    Serial.println(incomedate);
    if (incomedate == 'H')
    {
      Serial.println("Good Job!");
    }
  }

  delay(1000);

}
