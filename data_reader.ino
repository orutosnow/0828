const unsigned long BAUD_RATE = 115200;

void setup()
{
  Serial.begin(BAUD_RATE);
}

void loop()
{
  // Serialデータが来ているか確認
  if (Serial.available() > 0)
  {
    // 改行まで読み込む
    String receivedData = Serial.readStringUntil('\n');

    // 文字列 → float
    float acc = receivedData.toFloat();

    // 受信確認
    Serial.print("acc = ");
    Serial.println(acc, 6);
  }
}