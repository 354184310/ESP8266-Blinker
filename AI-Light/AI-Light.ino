#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT

#define Switch 2
#define Light 0

#include <Blinker.h>

char auth[] = "这里改为设备密钥";
char ssid[] = "这里改为WiFi名称";
char pswd[] = "这里改为WiFi密码";

// 新建组件对象
BlinkerButton Button("btn-light");

//声明全局变量
int counter = 0;
boolean i = 1;    //判断开关状态
boolean flag = 1;
boolean switchValue = 1;

// 按下按键执行该函数
void button_callback(const String & state)
{
  BLINKER_LOG("get button state: ", state);

  if (i) {
    digitalWrite(Light, HIGH);
    BLINKER_LOG("Turn 'ON' by APP");

    Button.color("#4BA632");
    Button.text("开");
    Button.print("on");
  }
  else {
    digitalWrite(Light, LOW);
    BLINKER_LOG("Turn 'OFF' by APP");

    Button.color("#D73022");
    Button.text("关");
    Button.print("off");
  }
  i = !i;
}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);
  counter++;
  BLINKER_LOG(counter);
}

//小爱语音控制
void miotPowerState(const String & state)
{
  BLINKER_LOG("need set power state: ", state);

  if (state == BLINKER_CMD_OFF) {
    digitalWrite(Light, LOW);
    BLINKER_LOG("Turn 'OFF' by MiAI");

    i = 1;

    BlinkerMIOT.powerState("on");
    BlinkerMIOT.print();

    Button.color("#D73022");
    Button.text("关");
    Button.print("off");
  }
  else if (state == BLINKER_CMD_ON) {
    digitalWrite(Light, HIGH);
    BLINKER_LOG("Turn 'ON' by MiAI");

    i = 0;

    BlinkerMIOT.powerState("off");
    BlinkerMIOT.print();

    Button.color("#4BA632");
    Button.text("开");
    Button.print("on");
  }
}

void miotQuery(int32_t queryCode) {
  BLINKER_LOG("MIOT Query codes: ", queryCode);
  switch (queryCode)
  {
    case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
      BLINKER_LOG("MIOT Query Power State");
      BlinkerMIOT.powerState(i ? "off" : "on");
      BlinkerMIOT.print();
      break;
  }
}

//板载按键控制
void keyCtrl() {
  if (!digitalRead(Switch) && flag) {
    delay(10);

    if (!digitalRead(Switch)) {
      flag = 0;

      if (i) {
        digitalWrite(Light, HIGH);
        BLINKER_LOG("Turn 'ON' by Switch");

        Button.color("#4BA632");
        Button.text("开");
        Button.print("on");
      }
      else {
        digitalWrite(Light, LOW);
        BLINKER_LOG("Turn 'OFF' by Switch");

        Button.color("#D73022");
        Button.text("关");
        Button.print("off");
      }
      i = !i;
    }
  }

  if (digitalRead(Switch) && !flag)
    flag = 1;
}

void setup()
{
  // 初始化串口
  Serial.begin(19200);
  BLINKER_DEBUG.stream(Serial);

  // 初始化IO
  pinMode(Switch, INPUT);
  pinMode(Light, OUTPUT);
  digitalWrite(Light, LOW);

  // 初始化blinker
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);

  //注册回调函数
  Button.attach(button_callback);
  BlinkerMIOT.attachPowerState(miotPowerState);

  //APP界面初始化
  Button.color("#D73022");
  Button.text("关");
  Button.print("off");
}

void loop() {
  Blinker.run();
  //板载按键控制，防止紧急情况
  keyCtrl();
}
