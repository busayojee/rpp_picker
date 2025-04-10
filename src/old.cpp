// One file code with basic 

// #include <Arduino.h>
// #include <EEPROM.h>
// #include <math.h>

// const int STEPS_PER_METER =5880; // approx, 
// enum OperationMode { MODE_NONE, DIRECT_MODE, IK_MODE };
// OperationMode currentMode = MODE_NONE;

// const int NUM_TASK_STEPS = 10;
// int taskSequence[NUM_TASK_STEPS][3] = {
//     {0, 0, 0},        
//     {0, 140, 50},     
//     {0, 140, 100},    
//     {0, 100, 100},    
//     {0, 0, 0},        
//     {90, 0, 0},       
//     {90, 140, 100},   
//     {90, 140, 50},    
//     {90, 0, 0},       
//     {0, 0, 0}        
// };
// int currentTaskStep = -1;
// unsigned long taskTimer = 0;
// bool taskInProgress = false;

// struct JointConfig {
//     uint8_t encCLK;
//     uint8_t encDT;
//     uint8_t servoPin;
//     volatile int32_t encoderPos;
//     int currentPosition;  
//     int targetPosition;
//     int stepsPerMeter; 
//     bool isLinear;
//     int JointLimit;
//     int jointNum;
// };
// bool inMove = false;
// const int NUM_JOINTS = 3;
// JointConfig joints[NUM_JOINTS] = {
//     {18, 19, 12, 0, 0, 0, 0, false, 180,1},      
//     {2, 3, 10, 0, 0, 0, STEPS_PER_METER, true, 200,2},     
//     {20, 21, 11, 0, 0, 0, STEPS_PER_METER, true, 100,3}    
// };

// const int16_t SERVO_NEUTRAL = -255;
// const int16_t SERVO_CW_MAX = -209;
// const int16_t SERVO_CCW_MAX = -300;
// const int DEGREES_PER_STEP = 6;
// const int TOLERANCE = 12;
// const int Tolerance_linear = 2;

// struct SavedData {
//     char signature[4] = "MEC";
//     int32_t encoderPositions[NUM_JOINTS];
//     float stepsPerMeter[NUM_JOINTS];
// };
// SavedData storedData;

// uint32_t lastEEPROMSave = 0;
// const uint32_t EEPROM_SAVE_INTERVAL = 5000;

// void promptOperationMode() {
//     Serial.println();
//     Serial.println("Select operation mode:");
//     Serial.println("D - Direct joint control");
//     Serial.println("I - Inverse kinematics control");
//     currentMode = MODE_NONE;
// }

// void handleEncoder(uint8_t joint) {
//     JointConfig &j = joints[joint];
//     uint8_t clk = digitalRead(j.encCLK);
//     uint8_t dt = digitalRead(j.encDT);
//     uint8_t encoded = (clk << 1) | dt;
//     static uint8_t lastEncoded[3] = {0};

//     uint8_t sum = (lastEncoded[joint] << 2) | encoded;

//     if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
//         j.encoderPos--;
//     } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
//         j.encoderPos++;
//     }
//     // Serial.println(j.encoderPos);
//     j.currentPosition = j.isLinear ? 
//         (j.encoderPos * 1000) / j.stepsPerMeter : 
//         (j.encoderPos * DEGREES_PER_STEP) % 360;
//     lastEncoded[joint] = encoded;
// }
// void encoderISR_J0() { handleEncoder(0); }
// void encoderISR_J1() { handleEncoder(1); }
// void encoderISR_J2() { handleEncoder(2); }

// void updateServo(uint8_t joint, int16_t value) {
//     // Serial.println(joints[joint].servoPin);
//     analogWrite(joints[joint].servoPin, value);
// }

// void handleModeSelection() {
//     if (Serial.available()) {
//         char mode = toupper(Serial.read());
//         if (mode == 'D') {
//             currentMode = DIRECT_MODE;
//             Serial.println("Selected: Direct Control Mode");
//         } else if (mode == 'I') {
//             currentMode = IK_MODE;
//             Serial.println("Selected: Inverse Kinematics Mode");
//         } else {
//             Serial.println("Invalid mode, choose D or I");
//             promptOperationMode();
//         }
//         while (Serial.available()) Serial.read();
//     }
// }

// void savePosition() {
//     for(int i=0; i<NUM_JOINTS; i++) {
//         storedData.encoderPositions[i] = joints[i].encoderPos;
//         storedData.stepsPerMeter[i] = joints[i].stepsPerMeter;
//     }
//     EEPROM.put(0, storedData);
// }

// void loadPosition() {
//     EEPROM.get(0, storedData);
    
//     if(strncmp(storedData.signature, "MEC", 3) != 0) {
//         for(int i=0; i<NUM_JOINTS; i++) {
//             joints[i].encoderPos = 0;
//             joints[i].stepsPerMeter = joints[i].isLinear ? STEPS_PER_METER : 0;
//         }
//         savePosition();
//     } else {
//         for(int i=0; i<NUM_JOINTS; i++) {
//             joints[i].encoderPos = storedData.encoderPositions[i];
//             joints[i].stepsPerMeter = storedData.stepsPerMeter[i];
//             joints[i].currentPosition = joints[i].isLinear ? 
//                 (joints[i].encoderPos * 1000) / joints[i].stepsPerMeter :
//                 (joints[i].encoderPos * DEGREES_PER_STEP) % 360;
//         }
//     }
// }

// int moveToTarget(uint8_t joint) {
//     JointConfig &j = joints[joint];
//     j.targetPosition = constrain(j.targetPosition, 0, j.JointLimit);
//     int error = j.targetPosition - j.currentPosition;
//     int tol = j.isLinear ? Tolerance_linear : TOLERANCE;
    
//     static unsigned long moveEndTime[NUM_JOINTS] = {0}; 

//     if (abs(error) <= tol) {
//         if (inMove) {
//             if (moveEndTime[joint] == 0) {
//                 moveEndTime[joint] = millis();
//             }
            
//             if (millis() - moveEndTime[joint] >= 100) { 
//                 updateServo(joint, SERVO_NEUTRAL);
//                 inMove = false;
//                 moveEndTime[joint] = 0;
//             }
//         } else {
//             updateServo(joint, SERVO_NEUTRAL);
//             return 0;
//         }
//     } 
//     else {
//         inMove = true;
//         moveEndTime[joint] = 0;
        
//         if (j.jointNum == 3) {
//             updateServo(joint, (error > 0) ? SERVO_CCW_MAX : SERVO_CW_MAX);
//         } else {
//             updateServo(joint, (error < 0) ? SERVO_CCW_MAX : SERVO_CW_MAX);
//         }

//         Serial.print("J");
//         Serial.print(joint+1);
//         Serial.print(": ");
//         Serial.print(j.currentPosition);
//         Serial.print(j.isLinear ? "mm" : "deg");
//         Serial.print(" -> ");
//         Serial.print(j.targetPosition);
//         Serial.print(j.isLinear ? "mm" : "deg");
//         Serial.print(" | ");
//         Serial.println();
        
//         return 1;
//     }
//     return 0;
// }

// void returnToZero() {
//     Serial.println("Homing all joints...");
//     for(int i=0; i<NUM_JOINTS; i++) {
//         int initialPosition = joints[i].currentPosition;
//         Serial.print("J");
//         Serial.print(i+1);
//         Serial.print(" initial: ");
//         Serial.print(initialPosition);
//         Serial.println(joints[i].isLinear ? "mm" : "deg");
        
//         joints[i].targetPosition = 0;
//         int tol = 0;
//         if(joints[i].isLinear){
//             tol = Tolerance_linear;
//         }
//         else{
//             tol = TOLERANCE;
//         }
//         while(abs(joints[i].currentPosition) > tol) {
//             moveToTarget(i);
//             // delay(10);
//         }
//         updateServo(i, SERVO_NEUTRAL);
//         joints[i].encoderPos = 0;
//     }
//     savePosition();
//     Serial.println("Homing complete");
//     promptOperationMode();
// }

// void runProgrammedTask() {
//     if (currentTaskStep == -1) return;
//     bool allReached = true;
//     for (int i = 0; i < NUM_JOINTS; i++) {
//         JointConfig &j = joints[i];
//         int error = abs(j.targetPosition - j.currentPosition);
//         int tol = j.isLinear ? Tolerance_linear : TOLERANCE;
//         if (error > tol) {
//             allReached = false;
//             break;
//         }
//     }

//     if (allReached) {
//         if (taskTimer == 0) {
//             taskTimer = millis();
//             int dwellTime = 5000; 
//             if (taskSequence[currentTaskStep][1] == 150 || taskSequence[currentTaskStep][2] == 100) { 
//                 dwellTime = 0; 
//             }

//             if (dwellTime > 0) {
//                 Serial.print("Dwelling for ");
//                 Serial.print(dwellTime/1000);
//                 Serial.println(" seconds");
//             }
//             else {
//                 currentTaskStep++;
//                 if (currentTaskStep >= NUM_TASK_STEPS) {
//                     Serial.println("Task sequence complete!");
//                     currentTaskStep = -1;
//                     taskInProgress = false;
//                     return;
//                 }
                
//                 for (int i = 0; i < NUM_JOINTS; i++) {
//                     joints[i].targetPosition = taskSequence[currentTaskStep][i];
//                 }
//                 taskTimer = 0;
//                 return;
//             }
//         }
//         else {

//             int dwellTime = 5000;
//             if (taskSequence[currentTaskStep][1] == 150 || taskSequence[currentTaskStep][2] == 100) {  
//                 dwellTime = 0;
//             }

//             if (dwellTime > 0 && (millis() - taskTimer >= dwellTime)) {
//                 currentTaskStep++;
//                 if (currentTaskStep >= NUM_TASK_STEPS) {
//                     Serial.println("Task sequence complete!");
//                     currentTaskStep = -1;
//                     taskInProgress = false;
//                     taskTimer = 0;
//                     return;
//                 }
//                 for (int i = 0; i < NUM_JOINTS; i++) {
//                     joints[i].targetPosition = taskSequence[currentTaskStep][i];
//                 }
//                 taskTimer = 0;
                
//                 Serial.print("Moving to step ");
//                 Serial.println(currentTaskStep);
//             }
//         }
//     }
//     else {
//         taskTimer = 0;
//     }
// }
// void startProgrammedTask() {
//     currentTaskStep = 0;
//     taskInProgress = true;
//     taskTimer = 0;
//     for (int i = 0; i < NUM_JOINTS; i++) {
//         joints[i].targetPosition = taskSequence[currentTaskStep][i];
//     }
//     Serial.println("Starting programmed task...");
// }

// void parseDirectCommands() {
//     static String inputBuffer;
//     while (Serial.available()) {
//         char c = Serial.read();
//         if (c == '\n') {
//             if (inputBuffer.equalsIgnoreCase("TASK")) {
//                 startProgrammedTask();
//             }
//             else{
//             int targets[3] = {0};
//             int index = 0;
//             inMove = true;
            
//             while (inputBuffer.length() > 0 && index < 3) {
//                 int commaIndex = inputBuffer.indexOf(',');
//                 if (commaIndex == -1) commaIndex = inputBuffer.length();
                
//                 targets[index] = inputBuffer.substring(0, commaIndex).toInt();
//                 inputBuffer = inputBuffer.substring(commaIndex + 1);
//                 index++;
//             }

//             for (int i = 0; i < NUM_JOINTS; i++) {
//                 joints[i].targetPosition = constrain(targets[i], 0, joints[i].JointLimit);
//             }
//             }
//             inputBuffer = "";
//         } else if (c != '\r') {
//             inputBuffer += c;
//         }
//     }
// }
// void InverseKinematics(double x, double y, double z, int *theta1, int *d2, int *d3){
//     *theta1 = (atan2(y,x))*57.3;
//      *d2 = z;
//      *d3 = sqrt(x*x + y*y);
// }

// void parseIKCommands() {
//     static String inputBuffer;
//     while (Serial.available()) {
//         char c = Serial.read();
//         if (c == '\n') {
//             float x = 0, y = 0, z = 0;
//             int index = 0;
//             inMove = true;
//             inputBuffer.trim();
            
//             while (inputBuffer.length() > 0 && index < 3) {
//                 int commaIndex = inputBuffer.indexOf(',');
//                 if (commaIndex == -1) commaIndex = inputBuffer.length();
                
//                 String token = inputBuffer.substring(0, commaIndex);
//                 switch (index) {
//                     case 0: x = token.toFloat(); break;
//                     case 1: y = token.toFloat(); break;
//                     case 2: z = token.toFloat(); break;
//                 }
//                 inputBuffer = inputBuffer.substring(commaIndex + 1);
//                 index++;
//             }
            
//             int theta1, d2, d3;
//             InverseKinematics(x, y, z, &theta1, &d2, &d3);
            
//             joints[0].targetPosition = constrain(round(theta1), 0, joints[0].JointLimit);
//             joints[1].targetPosition = constrain(round(d2), 0, joints[1].JointLimit);
//             joints[2].targetPosition = constrain(round(d3), 0, joints[2].JointLimit);
            
//             inputBuffer = "";
//         } else if (c != '\r') {
//             inputBuffer += c;
//         }
//     }
// }



// void setup() {
//     Serial.begin(9600);
//     for(int i=0; i<NUM_JOINTS; i++) {
//         pinMode(joints[i].encCLK, INPUT_PULLUP);
//         pinMode(joints[i].encDT, INPUT_PULLUP);
//         pinMode(joints[i].servoPin, OUTPUT);
        
//         switch(i) {
//             case 0: attachInterrupt(digitalPinToInterrupt(joints[0].encCLK), encoderISR_J0, CHANGE);attachInterrupt(digitalPinToInterrupt(joints[0].encDT), encoderISR_J0, CHANGE); break;
//             case 1: attachInterrupt(digitalPinToInterrupt(joints[1].encCLK), encoderISR_J1, CHANGE);attachInterrupt(digitalPinToInterrupt(joints[1].encDT), encoderISR_J1, CHANGE); break;
//             case 2: attachInterrupt(digitalPinToInterrupt(joints[2].encCLK), encoderISR_J2, CHANGE);attachInterrupt(digitalPinToInterrupt(joints[2].encDT), encoderISR_J2, CHANGE); break;
//         }
//     }
//     // analogWrite(11, -209);
//     loadPosition();
//     returnToZero();
//     promptOperationMode();
//     Serial.println("Ready to go");

// }

// void loop() {
//     // analogWrite(11, -209); //DOWN AND IN
//     if (currentMode == MODE_NONE) {
//         handleModeSelection();
//         return;
//     }
//     if (taskInProgress) {
//         runProgrammedTask();
//     }

//     switch (currentMode) {
//         case DIRECT_MODE:
//             parseDirectCommands();
//             break;
//         case IK_MODE:
//             parseIKCommands();
//             break;
//         default:
//             break;
//     }

  
//     bool anyMoving = false;
//     for(int i=0; i<NUM_JOINTS; i++) {
//         if(moveToTarget(i) != 0) anyMoving = true;
//     }
    
//     if(anyMoving) Serial.println(); 
    
//     if(millis() - lastEEPROMSave > EEPROM_SAVE_INTERVAL) {
//         savePosition();
//         lastEEPROMSave = millis();
//         Serial.println("Positions saved");
//     }
    

// }

  // 1470 = Down
    // 1476 = UP
    // analogWrite(11, -300); //UP AND OUT
    // delay(4000);
    
    
    // delay(1000);
    // analogWrite(12, -255);
    // delay(1000);
