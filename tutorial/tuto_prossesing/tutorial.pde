import processing.serial.*;
Serial port;// シリアルポート
int x; // x座標用
void setup(){
// ウィンドウサイズ
size(256, 256);
// ポートを初期化
port = new Serial(this, "/dev/cu.usbmodemF412FA63CEC02",115200);
// シリアルポートの初期化
port.clear();
}
void draw(){
// 背景色は白
background(255);
// 座標xに50*50の円を描く
ellipse(x,100,50,50);
}
// データが送信されてきたら呼び出される
void serialEvent(Serial p){
// ポートからデータを取得
x = p.read();
// 書き出して値を確認
println(x);
}
