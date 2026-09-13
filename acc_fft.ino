#include "fft.h"

const uint16_t SAMPLES = 64;

// 7 ms周期
const double SAMPLING_FREQUENCY = 142.857;

// FFT用配列
double vReal[SAMPLES];
double vImag[SAMPLES];

ArduinoFFT<double> FFT(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

int sampleIndex = 0;


// ==============================
// ウィリー判定設定
// ==============================

// 例：8～15 Hzを見る
const double FREQ_MIN = 8.0;
const double FREQ_MAX = 15.0;

// 仮の判定しきい値
double threshold = 10.0;


void setup()
{
  Serial.begin(115200);
}


void loop()
{
  // PCからデータが届いたら
  if (Serial.available() > 0)
  {
    String receivedData = Serial.readStringUntil('\n');

    double acc = receivedData.toFloat();


    // ============================
    // 64点保存
    // ============================

    vReal[sampleIndex] = acc;
    vImag[sampleIndex] = 0.0;

    sampleIndex++;


    // ============================
    // 64点たまったらFFT
    // ============================

    if (sampleIndex >= SAMPLES)
    {
      calculateFFT();

      sampleIndex = 0;
    }
  }
}


// ==================================
// FFT処理
// ==================================

void calculateFFT()
{
  // ------------------------------
  // ① 平均値を求める
  // ------------------------------

  double mean = 0.0;

  for (int i = 0; i < SAMPLES; i++)
  {
    mean += vReal[i];
  }

  mean /= SAMPLES;


  // ------------------------------
  // ② 平均値を引く
  // DC成分除去
  // ------------------------------

  for (int i = 0; i < SAMPLES; i++)
  {
    vReal[i] -= mean;
  }


  // ------------------------------
  // ③ 窓関数
  // ------------------------------

  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);


  // ------------------------------
  // ④ FFT
  // ------------------------------

  FFT.compute(FFTDirection::Forward);


  // ------------------------------
  // ⑤ 複素数 → 振幅
  // ------------------------------

  FFT.complexToMagnitude();


  // ------------------------------
  // ⑥ 指定周波数帯の強度を合計
  // ------------------------------

  double bandPower = 0.0;

  for (int i = 1; i < SAMPLES / 2; i++)
  {
    double frequency =
        i * SAMPLING_FREQUENCY / SAMPLES;

    if (frequency >= FREQ_MIN &&
        frequency <= FREQ_MAX)
    {
      bandPower += vReal[i];
    }
  }


  // ------------------------------
  // ⑦ 結果表示
  // ------------------------------

  Serial.print("BandPower = ");
  Serial.print(bandPower, 4);


  // ------------------------------
  // ⑧ ウィリー判定
  // ------------------------------

  if (bandPower > threshold)
  {
    Serial.println(" , WHEELIE");
  }
  else
  {
    Serial.println(" , NORMAL");
  }
}