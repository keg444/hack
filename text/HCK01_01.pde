void setup(){
  size(480, 120);
}

void draw(){
  if (mousePressed){
    fill(0);
  }else{
    fill(255);
  }
  ellipse(mouseX, mouseY, 80, 80);
  //background(#ff99ff);
  //stroke(#ff0000);
  //line(0,0, 480,120);
}

void keyPressed(){
  print(key);
}
