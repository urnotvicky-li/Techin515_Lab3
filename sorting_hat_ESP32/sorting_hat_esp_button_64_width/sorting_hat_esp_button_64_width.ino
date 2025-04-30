#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sorting_hat_model.h"  // ML Model

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Button Pins
#define BUTTON_A  13
#define BUTTON_B  12
#define BUTTON_C  14
#define BUTTON_D  27

const int buttons[] = {BUTTON_A, BUTTON_B, BUTTON_C, BUTTON_D};
const int buttonLabels[] = {1, 2, 3, 4};
const int numButtons = 4;

// Sorting Hat Questions and Answers
const char* questions[] = {
    "1. What do you value?",
    "2. What to do if someone cheats?",
    "3. Favorite subject?",
    "4. How do you face challenges?",
    "5. How do friends describe you?",
    "6. What to do with a mystery book?",
    "7. Preferred pet?",
    "8. How do you solve problems?",
    "9. What kind of friends do you like?",
    "10. Dream career?"
};

const char* options[][4] = {
    {"A) Bravery", "B) Loyalty", "C) Intelligence", "D) Ambition"},
    {"A) Call them out", "B) Let them be", "C) Inform teacher", "D) Gain from it"},
    {"A) Defense Arts", "B) Herbology", "C) Charms", "D) Potions"},
    {"A) Face head-on", "B) Team up", "C) Plan first", "D) Outsmart it"},
    {"A) Bold", "B) Kind", "C) Smart", "D) Resourceful"},
    {"A) Read it now", "B) Check safety", "C) Study it", "D) Use for gain"},
    {"A) Owl", "B) Toad", "C) Cat", "D) Phoenix"},
    {"A) Act fast", "B) Find compromise", "C) Analyze first", "D) Outsmart"},
    {"A) Adventurous", "B) Loyal", "C) Thoughtful", "D) Powerful"},
    {"A) Auror", "B) Healer", "C) Scholar", "D) Minister"}
};

// State
int responses[10] = {0};
int questionIndex = 0;
Eloquent::ML::Port::DecisionTree clf;

void setup() {
    Serial.begin(115200);

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println("SSD1306 allocation failed");
        while (true);
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(10, 10);
    display.println("Sorting Hat Ready!");
    display.display();
    delay(2000);

    for (int i = 0; i < numButtons; i++) {
        pinMode(buttons[i], INPUT_PULLUP);
    }

    showQuestion();
}

void loop() {
    checkButtons();
}

void showQuestion() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(questions[questionIndex]);

    for (int i = 0; i < 4; i++) {
        display.setCursor(10, 20 + i * 10);
        display.println(options[questionIndex][i]);
    }

    display.display();
}

void checkButtons() {
    for (int i = 0; i < numButtons; i++) {
        if (digitalRead(buttons[i]) == LOW) {  // Button pressed
            responses[questionIndex] = buttonLabels[i];  // Record response
            Serial.print("Q"); Serial.print(questionIndex + 1);
            Serial.print(" Answered: "); Serial.println(buttonLabels[i]);

            delay(300);  // Debounce
            nextQuestion();
            break;
        }
    }
}

void nextQuestion() {
    questionIndex++;
    if (questionIndex < 10) {
        showQuestion();
    } else {
        classifyHouse();
    }
}

void classifyHouse() {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Sorting...");
    display.display();
    delay(1500);

    float features[10];
    for (int i = 0; i < 10; i++) {
        features[i] = (float)responses[i];
    }

    int house = clf.predict(features);

    display.clearDisplay();
    display.setCursor(10, 10);
    display.print("House: ");
    switch (house) {
        case 0: display.println("Gryffindor"); break;
        case 1: display.println("Hufflepuff"); break;
        case 2: display.println("Ravenclaw"); break;
        case 3: display.println("Slytherin"); break;
        default: display.println("Unknown"); break;
    }
    display.display();

    Serial.println("Sorting complete!");
    Serial.print("Predicted House: ");
    switch (house) {
        case 0: Serial.println("Gryffindor"); break;
        case 1: Serial.println("Hufflepuff"); break;
        case 2: Serial.println("Ravenclaw"); break;
        case 3: Serial.println("Slytherin"); break;
    }
}
