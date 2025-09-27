/*
 * Project: AGV 
 * Author: boan
 */
// 2.狀態燈: 綠燈(通電恆亮)、紅燈(異常閃爍)、橘燈(方向燈)*2、白燈(倒退恆亮)、黃燈(超聲波閃爍)

// --------------- 腳位命名 ---------------
// 輸送帶
#define MOTOR     19     // 繼電器
//#define TCRT_IN   3     // 紅外線(輸送帶入口)
#define TCRT_OUT  18     // 紅外線(輸送帶出口)

// 狀態燈
#define GREEN     10     // 綠燈: 通電恆亮
#define RED       11     // 紅燈: 異常(障礙物)閃爍
#define ORANGE_L  12    // 橘燈: 左邊閃爍
#define ORANGE_R  13     // 橘燈: 右邊閃爍
#define YELLOW   14     // 黃燈: 超聲波運作過程閃爍


// ---------------- 狀態旗標 ----------------
bool blinkLeft  = false;
bool blinkRight = false;
bool blinkRed  = false;
bool blinkYellow  = false;
// --- 輸送帶狀態 ---
bool relay_on = true;


// --------------- 設定與初始化 ---------------
void setup() {

  // 鮑率(baud rate):   每秒傳輸多少個符號
  // 位元率 (bit rate): 每秒傳輸多少位元數
  Serial.begin(115200);     // PICO Console(除錯用)
  Serial1.begin(115200);    // UART 與 Rpi 通訊
  
  // 狀態燈(輸出供電)
  pinMode(GREEN,    OUTPUT);
  pinMode(RED,      OUTPUT);
  pinMode(ORANGE_L, OUTPUT);
  pinMode(ORANGE_R, OUTPUT);
  pinMode(YELLOW,   OUTPUT);
  // 輸送帶
  pinMode(MOTOR, OUTPUT);//繼電器
  pinMode(TCRT_OUT, INPUT);//紅外線

  // 初始化狀態(全部關燈，保留綠燈)
  digitalWrite(GREEN,    HIGH);      // 通電恆亮
  digitalWrite(MOTOR,    HIGH);
  digitalWrite(RED,      LOW);
  digitalWrite(ORANGE_L, LOW);
  digitalWrite(ORANGE_R, LOW);
  digitalWrite(YELLOW,   LOW);

  Serial1.println("Pico Ready!");
}


// -------------------- 方向燈閃爍(非阻塞閃爍設定) --------------------

// 左方向燈閃爍
unsigned long prevMillisLeft = 0;   // 上一次左橘燈切換的時間
bool leftState = LOW;               // 左橘燈狀態
const long intervalLeft = 500;      // 左橘燈閃爍間隔 (毫秒)

// 右方向燈閃爍
unsigned long prevMillisRight = 0;  // 上一次右橘燈切換的時間
bool rightState = LOW;              // 右橘燈狀態
const long intervalRight = 500;     // 右橘燈閃爍間隔 (毫秒)

// 紅燈(異常)閃爍
unsigned long prevMillisRed = 0;    // 上一次紅燈切換時間
bool redState = LOW;                // 紅燈狀態
const long intervalRed = 500;       // 紅燈閃爍間隔

// 黃燈(超聲波)閃爍
unsigned long prevMillisYellow = 0; // 上一次黃燈切換時間
bool yellowState = LOW;             // 黃燈狀態
const long intervalYellow = 300;    // 黃燈閃爍間隔 (可以比方向燈快)

unsigned long prevMillisBoth = 0;  // 上一次雙邊閃爍時間
bool bothState = LOW;              // 雙邊橘燈狀態
const long intervalBoth = 500;     // 閃爍間隔 (毫秒)


//輸送帶繼電器
void MotorControl();
void controlRelay(bool on);

// --------------- 主程式(無限迴圈) ---------------
void loop() {

  if (Serial1.available()) {
    char cmd = Serial1.read();

    switch(cmd) {
      case 'R': 
        blinkRed=true;; 
        Serial1.println("Red ON"); 
        break;

      case 'r': 
        blinkRed = false; 
        digitalWrite(RED,      LOW);
        Serial1.println("Red OFF"); 
        break;

      case 'L': 
        blinkLeft=true;  
        Serial1.println("Left ON"); 
        break;

      case 'l': 
        blinkLeft = false;
        digitalWrite(ORANGE_L, LOW);  
        Serial1.println("Left OFF"); 
        break;

      case 'O':  
        blinkRight=true; 
        Serial1.println("Right ON"); 
        break;

      case 'o': 
        blinkRight=false;
        digitalWrite(ORANGE_R, LOW);   
        Serial1.println("Right OFF"); 
        break;

      case 'Y': 
        blinkYellow=true;
        digitalWrite(YELLOW, HIGH); 
        yellowState=true; 
        Serial1.println("Yellow ON"); 
        break;

      case 'y': 
        blinkYellow=false;
        digitalWrite(YELLOW, LOW);  
        yellowState=false; 
        Serial1.println("Yellow OFF"); 
        break;

      case 'Z':
          relay_on = false;
          controlRelay(false); // // 關閉 relay
          Serial1.println("motor ON");
        break;

      default: 
        Serial1.println("Unknown");  
        break;
    }
  }
  MotorControl();

        unsigned long now = millis();

        // ------------------- 左橘燈閃爍 -------------------
    if (blinkLeft) {
        if (now - prevMillisLeft >= intervalLeft) {
            prevMillisLeft = now;
            leftState = !leftState;
            digitalWrite(ORANGE_L, leftState);
        }
    }

    // ------------------- 右橘燈閃爍 -------------------
    if (blinkRight) {
        if (now - prevMillisRight >= intervalRight) {
            prevMillisRight = now;
            rightState = !rightState;
            digitalWrite(ORANGE_R, rightState);
        }
    }
   // ------------------- 紅燈閃爍 -------------------
    if (blinkRed) {
        if (now - prevMillisRed >= intervalRed) {
            prevMillisRed = now;
            redState = !redState;
            digitalWrite(RED, redState);
        }
    }
}

void MotorControl() {
  if (!relay_on) {
    int val2 = digitalRead(TCRT_OUT);
    if (val2 == 0) { // IR2 被觸發
      controlRelay(true);
      relay_on = true;  
      Serial1.println("motor OFF");
    }
  }
}

void controlRelay(bool on) {
  digitalWrite(MOTOR, on ? HIGH : LOW);
}