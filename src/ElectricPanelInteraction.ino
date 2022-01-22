const uint8_t bananaSocketInputPin1 = 2, bananaSocketInputPin2 = 3, bananaSocketInputPin3 = 4, bananaSocketInputPin4 = 5;
const uint8_t bananaSocketOutputPin1 = 6, bananaSocketOutputPin2 = 7, bananaSocketOutputPin3 = 8, bananaSocketOutputPin4 = 9;

bool INTERACTION_SOLVED, INTERACTION_RUNNING;

void setup() {
  Serial.begin(9600);
  pinMode(bananaSocketOutputPin1, OUTPUT);
  pinMode(bananaSocketInputPin1, INPUT);
  pinMode(bananaSocketOutputPin2, OUTPUT);
  pinMode(bananaSocketInputPin2, INPUT);
  pinMode(bananaSocketOutputPin3, OUTPUT);
  pinMode(bananaSocketInputPin3, INPUT);
  pinMode(bananaSocketOutputPin4, OUTPUT);
  pinMode(bananaSocketInputPin4, INPUT);
}

void loop() {
  if (!Serial) {
    Serial.begin(9600);
  }
  if (Serial.available()) {
    processSerialMessage();
  }
  if (INTERACTION_SOLVED == false && INTERACTION_RUNNING == true) {
    gameLoop();
  }
}

void gameLoop() {
  digitalWrite(bananaSocketOutputPin1, HIGH);
  if (digitalRead(bananaSocketInputPin1) == HIGH) {
    digitalWrite(bananaSocketOutputPin1, LOW);
    delay(500);
    digitalWrite(bananaSocketOutputPin2, HIGH);
    if (digitalRead(bananaSocketInputPin2) == HIGH) {
      digitalWrite(bananaSocketOutputPin2, LOW);
      delay(500);
      digitalWrite(bananaSocketOutputPin3, HIGH);
      if (digitalRead(bananaSocketInputPin3) == HIGH) {
        digitalWrite(bananaSocketOutputPin3, LOW);
        delay(500);
        digitalWrite(bananaSocketOutputPin4, HIGH);
        if (digitalRead(bananaSocketInputPin4) == HIGH) {
          digitalWrite(bananaSocketOutputPin4, LOW);
          checkWinning();
          INTERACTION_SOLVED = true;
        }
      }
    }
  }
  delay(500);
}

void processSerialMessage() {
  const uint8_t BUFF_SIZE = 64; // make it big enough to hold your longest command
  static char buffer[BUFF_SIZE + 1]; // +1 allows space for the null terminator
  static uint8_t length = 0; // number of characters currently in the buffer

  char c = Serial.read();
  if ((c == '\r') || (c == '\n')) {
    // end-of-line received
    if (length > 0) {
      tokenizeReceivedMessage(buffer);
    }
    length = 0;
  } else {
    if (length < BUFF_SIZE) {
      buffer[length++] = c; // append the received character to the array
      buffer[length] = 0; // append the null terminator
    }
  }
}

void tokenizeReceivedMessage(char *msg) {
  const uint8_t COMMAND_PAIRS = 10;
  char* tokenizedString[COMMAND_PAIRS + 1];
  uint8_t index = 0;

  char* command = strtok(msg, ";");
  while (command != 0) {
    char* separator = strchr(command, ':');
    if (separator != 0) {
      *separator = 0;
      tokenizedString[index++] = command;
      ++separator;
      tokenizedString[index++] = separator;
    }
    command = strtok(0, ";");
  }
  tokenizedString[index] = 0;

  processReceivedMessage(tokenizedString);
}

void processReceivedMessage(char** command) {
  if (strcmp(command[1], "START") == 0) {
    startSequence(command[3]);
  } else if (strcmp(command[1], "PAUSE") == 0) {
    pauseSequence(command[3]);
  } else if (strcmp(command[1], "STOP") == 0) {
    stopSequence(command[3]);
  } else if (strcmp(command[1], "INTERACTION_SOLVED_ACK") == 0) {
    setInteractionSolved();
  } else if (strcmp(command[1], "PING") == 0) {
    ping(command[3]);
  } else if (strcmp(command[1], "BAUD") == 0) {
    setBaudRate(atoi(command[3]), command[5]);
  } else if (strcmp(command[1], "SETUP") == 0) {
    Serial.println("COM:SETUP;INT_NAME:Electric Panel Interaction;BAUD:9600");
    Serial.flush();
  }
}

//TODO: Review This Method once Interaction Is Completed
void startSequence(char* TIMESTAMP) {
  INTERACTION_SOLVED = false;
  INTERACTION_RUNNING = true;
  Serial.print("COM:START_ACK;MSG:1º(2/6) 2º(3/7) 3º(4/8) 4º(5/9)");
  Serial.print(";ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void pauseSequence(char* TIMESTAMP) {
  INTERACTION_RUNNING = !INTERACTION_RUNNING;
  if (INTERACTION_RUNNING) {
    Serial.print("COM:PAUSE_ACK;MSG:Device is now running;ID:");
  } else {
    Serial.print("COM:PAUSE_ACK;MSG:Device is now paused;ID:");
  }
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void stopSequence(char* TIMESTAMP) {
  INTERACTION_RUNNING = false;
  Serial.print("COM:STOP_ACK;MSG:Device is now stopped;ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void setInteractionSolved() {
  INTERACTION_SOLVED = true;
  INTERACTION_RUNNING = false;
}

void ping(char* TIMESTAMP) {
  Serial.print("COM:PING;MSG:PING;ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

void setBaudRate(int baudRate, char* TIMESTAMP) {
  Serial.flush();
  Serial.begin(baudRate);
  Serial.print("COM:BAUD_ACK;MSG:The Baud Rate was set to ");
  Serial.print(baudRate);
  Serial.print(";ID:");
  Serial.print(TIMESTAMP);
  Serial.print("\r\n");
  Serial.flush();
}

bool checkWinning() {
  Serial.println("COM:INTERACTION_SOLVED;MSG:User Connected Wires Correctly;PNT:750");
  Serial.flush();
}
