// Biblioteca FastLED 3.5.0 (Daniel Garcia)
#include <FastLED.h>

#define DATA_PIN D4
#define LARGURA 16
#define ALTURA 16
#define NUM_LEDS (LARGURA * ALTURA)
// Truquezinho ;)
#define XY_(X, Y) ( ((X) << 4) + ( ((X) & 1) ? (15 - (Y)) : (Y) ) )
#define XY(X, Y) XY_((15 - (X)), (15 - (Y)))

CRGB leds[NUM_LEDS];

int vez = 0;
int pontos = 0;
int xRaquete = 0;
#define yRaquete 14
#define larguraRaquete 3
float xBola = 0.0f;
float yBola = 1.0f;
float vxBola = 0.0f;
float vyBola = 0.5f;
long horaAnterior;
#define botaoE D1
#define botaoD D2
byte estadoAnteriorE = 0;
byte estadoAnteriorD = 0;

void limparLeds() {
  for (int i = 0; i < NUM_LEDS; i = i + 1) {
    leds[i] = CHSV((byte)(i + vez), 255, 50);
  }
  for (int i = 0; i < LARGURA; i = i + 1) {
    leds[XY(i, 0)] = CRGB(128, 128, 128);
  }
}

void moverBola(float novoX, float novoY) {
  int novoXInteiro = (int)novoX;
  int novoYInteiro = (int)novoY;

  leds[XY(novoXInteiro, novoYInteiro)] = CRGB(128, 128, 128);

  xBola = novoX;
  yBola = novoY;
}

void desenharRaquete() {
  leds[XY(xRaquete, yRaquete)] = CRGB(128, 128, 128);
  leds[XY(xRaquete + 1, yRaquete)] = CRGB(128, 128, 128);
  leds[XY(xRaquete + 2, yRaquete)] = CRGB(128, 128, 128);
}

void setup() {
  Serial.begin(115200);

  pinMode(botaoE, INPUT);
  pinMode(botaoD, INPUT);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(255); // Vai de 0 a 255

  randomSeed(analogRead(A0));
  vxBola = (float)(random(201) - 100) / 100.0f; //random.randint(-100, 100) / 100
  vyBola = (float)(random(91) + 10) / 100.0f; //random.randint(10, 100) / 100

  horaAnterior = millis();
}

void loop() {
  vez = vez + 4;
  limparLeds();

  if (digitalRead(botaoE)) {
    if (estadoAnteriorE == 0) {
      estadoAnteriorE = 1;
      if (xRaquete > 0) {
        xRaquete = xRaquete - 1;
      }
    }
  } else {
    estadoAnteriorE = 0;
  }

  if (digitalRead(botaoD)) {
    if (estadoAnteriorD == 0) {
      estadoAnteriorD = 1;
      if (xRaquete < (LARGURA - larguraRaquete)) {
        xRaquete = xRaquete + 1;
      }
    }
  } else {
    estadoAnteriorD = 0;
  }

  long horaAtual = millis();
  float deltaT = (float)(horaAtual - horaAnterior) / 175.0f;
  horaAnterior = horaAtual;

  float novoX = xBola + (vxBola * deltaT);
  int novoXInteiro = (int)novoX;
  if (novoXInteiro < 0) {
      // Bateu na parede da esquerda
      novoX = 0.0f;
      vxBola = -vxBola;
  } else if (novoXInteiro > 15) {
      // Bateu na parede da direita
      novoX = 15.0f;
      vxBola = -vxBola;
  }

  float novoY = yBola + (vyBola * deltaT);
  int novoYInteiro = (int)novoY;
  if (novoYInteiro <= 0) {
    // Bateu no teto
    novoY = 1.0f;
    vxBola = (float)(random(201) - 100) / 100.0f; //random.randint(-100, 100) / 100
    vyBola = (float)(random(91) + 10) / 100.0f; //random.randint(10, 100) / 100
    pontos = pontos + 1;
    Serial.print("Ponto! ");
    Serial.println(pontos);
  } else if (novoYInteiro == yRaquete && novoXInteiro >= xRaquete && novoXInteiro <= (xRaquete + 2)) {
    // Rebateu!!!! :)
    novoY = yRaquete - 1;
    vxBola = (float)(random(201) - 100) / 100.0f; //random.randint(-100, 100) / 100
    vyBola = (float)(random(91) + 10) / -100.0f; //random.randint(10, 100) / -100
    pontos = pontos + 2;
    Serial.print("Ponto! ");
    Serial.println(pontos);
  } else if (novoYInteiro > 15) {
    // Game Over
    Serial.print("Game Over :( ");
    Serial.println(pontos);
    delay(5000);
    pontos = 0;
    xRaquete = 0;
    novoX = 0.0f;
    novoY = 1.0f;
    vxBola = (float)(random(201) - 100) / 100.0f; //random.randint(-100, 100) / 100
    vyBola = (float)(random(91) + 10) / 100.0f; //random.randint(10, 100) / 100
    horaAnterior = millis();
  }

  moverBola(novoX, novoY);
  desenharRaquete();
  FastLED.show();
  delay(50);
}
