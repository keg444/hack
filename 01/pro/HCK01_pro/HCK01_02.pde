import processing.serial.*;

Serial port;
int [][] frame = new int[8][12];

void setup(){
  size(256,256);
  port = new Serial(this, "/dev/cu.usbmodemF412FA63CEC02",115200);
  port.clear();
}

void draw(){
  background(255);
  //1ドットの大きさ
  int w = 10;
  int h = 10;
  
  for(int i=0; i<8; i++){
    for(int j=0; j<12; j++){
      if(frame[i][j] == 1){
        fill(0);  //黒く塗る
        rect((width/3)+j*w, (height/3)+i*h, w, h);
      }else{
        //fill(255);　　//白く塗る
      }
    }
  }
}

void serialEvent(Serial p){
  if(p.available() >=96){
    for(int i=0; i<8; i++){
      for (int j=0; j<12; j++){
        frame[i][j] = p.read();
        //print(frame[i][j]);
      }
    }
  }
}
