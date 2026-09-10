import serial
import time
import openpyxl

# ==============================
# 設定
# ==============================

EXCEL_FILE = "acc_data.xlsx"
SHEET_NAME = None       # Noneならアクティブシート
SERIAL_PORT = "COM3"    # ArduinoのCOMポートに変更
BAUD_RATE = 115200

INTERVAL = 0.007        # 7 ms = 0.007 s
START_ROW = 2           # 1行目が見出しの場合
DATA_COLUMN = "D"


# ==============================
# Excel読み込み
# ==============================
## Excelファイルを開く
##（data_only=True:数式ではなく、Excelが保存している計算結果を取得）
wb = openpyxl.load_workbook(EXCEL_FILE, data_only=True)

## シート名を指定していない場合、現在アクティブになっているシートを使用
if SHEET_NAME is None:
    ws = wb.active
else:
    ws = wb[SHEET_NAME]


## D列の速度データを読み込む
acc_data = []

for row in range(START_ROW, ws.max_row + 1):
    value = ws[f"{DATA_COLUMN}{row}"].value

    if value is not None:
        try:
            acc_data.append(float(value))
        except (ValueError, TypeError):
            print(f"{row}行目をスキップしました: {value}")


print(f"読み込みデータ数: {len(acc_data)}")


# ==============================
# Arduino接続
# ==============================

ser = serial.Serial(
    SERIAL_PORT,
    BAUD_RATE,
    timeout=1
)

# Arduinoのリセット待ち
time.sleep(2)

# 受信バッファをクリア
ser.reset_input_buffer()
ser.reset_output_buffer()

print("送信開始")


# ==============================
# 7ms間隔で送信
# ==============================

start_time = time.perf_counter()

for i, acc in enumerate(acc_data):

    # 本来このデータを送信する時刻
    target_time = start_time + i * INTERVAL

    # 予定時刻まで待つ
    while time.perf_counter() < target_time:
        pass

    # Arduinoへ送信
    message = f"{acc}\n"
    ser.write(message.encode("utf-8"))

    # PC側でも確認
    elapsed_time = time.perf_counter() - start_time

    print(
        f"{i:5d}, "
        f"time = {elapsed_time:.6f} s, "
        f"acc = {acc}"
    )


print("送信終了")

ser.close()