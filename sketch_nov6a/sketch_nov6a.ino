void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);

    Serial.println("Hello World!");
}

void loop()
{
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}