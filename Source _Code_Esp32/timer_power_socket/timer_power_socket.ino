#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;


#define WIFI_SSID "iPhone of H"
#define WIFI_PASSWORD "123456789@@@"

#define FIREBASE_HOST "timer-power-socket-dab07-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "8X9DNQPhr3ZOelYmiZ2a0lHr0AUBz5C2LyhyZrDr"



unsigned long t1 = 0;
int GT = 0, GTS = 0;
byte giocd, phutcd;
FirebaseData firebaseData;

NTPClient timeClient(ntpUDP, "pool.ntp.org");
byte ngay, thang, gio, phut, giay;
unsigned int nam;
String path = "/";
bool ena = 0, k = 0;

// Chân kết nối relay
const int RELAY1_PIN = 4; // Chân kết nối relay 1
const int RELAY2_PIN = 5; // Chân kết nối relay 2

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Kết nối WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to IP address: ");
  Serial.println(WiFi.localIP());

  // Khởi tạo NTPClient
  timeClient.begin();
  timeClient.setTimeOffset(25200);

  // Khởi tạo Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true); // Đảm bảo kết nối vẫn sống

  // Thiết lập chân điều khiển relay
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
}

void loop() {
  timeClient.update();//thời gian từ NTPClient được cập nhật bằng cách gọi hàm timeClient.update()

  unsigned long epochTime = timeClient.getEpochTime();
  String formattedTime = timeClient.getFormattedTime();
  struct tm* ptm = gmtime((time_t*)&epochTime);

  ngay = ptm->tm_mday;//Lấy giá trị ngày từ cấu trúc thời gian và gán vào biến ngay.
  thang = ptm->tm_mon + 1;// Lấy giá trị tháng từ cấu trúc thời gian và gán vào biến thang. tháng tuwef 0-1, t cộng thêm 1 sử để đc tháng thực tế
  nam = ptm->tm_year + 1900;

  gio = timeClient.getHours();
  phut = timeClient.getMinutes();
  giay = timeClient.getSeconds();

//kiểm tra dữ liệu đc nhập từ serial . nếu đọc gửi kiểm tra là 1
//biến ena = 1; đảo trạng thái, bật tắt thông qua phím
  if (Serial.available()) {
    char c = Serial.read();
    Serial.println("Nhap tu ban phim!");
    if (c == '1') {
      ena = !ena;
    }
  }

    if (millis() - t1 > 1000) {
    if (Firebase.getInt(firebaseData, path + "hour"))
      giocd = firebaseData.intData();
    if (Firebase.getInt(firebaseData, path + "minute"))
      phutcd = firebaseData.intData();
    
    if (gio==giocd) {
      if (phut == phutcd && k == 0) {
        ena=1;
        k = 1;
   
      }
    if (phut!=phutcd) {
        k = 0;
    }
    }
    if(ena==1)
    {
          digitalWrite(RELAY1_PIN, HIGH);
        digitalWrite(RELAY2_PIN, HIGH);
        Serial.println("Da bat relay!");
    }
    else {
    
        digitalWrite(RELAY1_PIN, LOW);
        digitalWrite(RELAY2_PIN, LOW);
      Serial.println("Da tat relay")  ;
        }

  }
}


