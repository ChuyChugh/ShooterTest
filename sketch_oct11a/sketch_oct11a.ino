#include <HardwareSerial.h>
#include <ODriveArduino.h>

template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

HardwareSerial& odrive1_serial = Serial1;

ODriveArduino odrive1(odrive1_serial);

void setup() {
  odrive1_serial.begin(115200);
  
  Serial.begin(115200);
  while(!Serial){
    Serial.println("ODriveArduino");
    Serial.println("Setting parameters...");
  }
  for(int i = 0; i < 2; ++i){
    Serial1 << "w axis" << i << ".controller.config.vel_limit" << 22000.0f << '\n';
    Serial1 << "w axis" << i << ".controller.config.current_lim" << 11.0f << '\n';
  }
  
  Serial.println("Ready!");
  Serial.println("Send the character '0' or '1' to calibrate respective motor (you must do this before you can command movement)");
  Serial.println("Send the character 's' to exectue test move");
  Serial.println("Send the character 'b' to read bus voltage");
  Serial.println("Send the character 'p' to do a spinning test");
}

void loop() {
  char character = Serial.read();
  if(character == '0' || character == 'l'){
    int num = (int)character;
    int requested_state = ODriveArduino::AXIS_STATE_MOTOR_CALIBRATION;
    Serial << "Axis" << character << ": Requesting state " << requested_state << '\n';
    if(!odrive1.run_state(num, requested_state, true)) return;

    requested_state = ODriveArduino::AXIS_STATE_ENCODER_OFFSET_CALIBRATION;
    Serial << "Axis" << character << ": Requesting state " << requested_state << '\n';
    if(!odrive1.run_state(num, requested_state, true, 25.0f)) return;

    requested_state = ODriveArduino::AXIS_STATE_CLOSED_LOOP_CONTROL;
    Serial << "Axis" << character << ": Requesting state " << requested_state << '\n';
    if(!odrive1.run_state(num, requested_state, false /*don't wait*/)) return;
  }
  if(character == 'b'){
    odrive1_serial  << "r vbus_voltage\n";
    Serial << "VBus Voltage for ODrive1: " << odrive1.readFloat() << '\n';
  }
  if(character == 'p'){
    float pos_m0 = 200000.0f;
    odrive1.SetPosition(0, pos_m0);
    delay(5000);
    pos_m0 = 0.0f;
    odrive1.SetPosition(0, pos_m0);
    delay(5000);

    int axis = 0;
    Serial1 << "w axis" << 0 << ".controller.config.vel_limit " << 200000.0f << '\n';

    pos_m0 = 500000.0f;
    odrive1.SetPosition(0, pos_m0);
    delay(5000);
    pos_m0 = 0.0f;
    odrive1.SetPosition(0, pos_m0);
  }
}
