#include "esphome.h"

class JiecangDeskSensor : public PollingComponent, public UARTDevice {
    private:
        float physical_min = 0;
        float physical_max = 0;

    public:
        JiecangDeskSensor(UARTComponent *parent) : UARTDevice(parent) {}

        Sensor *height = new Sensor();
        Sensor *unit = new Sensor();

        Sensor *height_min = new Sensor();
        Sensor *height_max = new Sensor();

        Sensor *height_pct = new Sensor();

        Sensor *position1 = new Sensor();
        Sensor *position2 = new Sensor();
        Sensor *position3 = new Sensor();
        Sensor *position4 = new Sensor();


        void setup() override {
            // request limits
            // unsigned int messages[] = {0xF1, 0xF1, 0x07, 0x00, 0x07, 0x7e};
            // for(auto byte : messages) write(byte);
        }

        bool bufferMessage(int readch, unsigned int *buffer, int len)
        {
            static bool cmd_incoming = false;
            static int pos = 0;

            switch (readch) {
                case 0xF2: // start of message
                    cmd_incoming = true;
                    break;
                case 0x7e: // end of message
                    cmd_incoming = false;
                    for (;pos<len-1;pos++) { // fill rest of buffer with zeros
                        buffer[pos]=0;
                    }
                    pos = 0;
                    return true;
                default:
                    if (cmd_incoming && pos < len) {
                        buffer[pos++] = readch;
                    }
            }
            if (cmd_incoming && pos >= len) {
                cmd_incoming=false;
            }
            return false;
        }

        float byte2float(int high, int low) {
            return static_cast<float>((high<<8) + low)/10;
        }

        void handleMessage(unsigned int *message) {
            // ESP_LOGV("jiecang_desk_sensor", "message %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", message[0], message[1], message[2], message[3], message[4], message[5], message[6], message[7], message[8], message[9]);

            if (message[0] == 1) {
                ESP_LOGV("jiecang_desk_sensor", "height 0x%0X%0X", message[2], message[3]);
                height->publish_state(byte2float(message[2], message[3]));
                height_pct->publish_state((height->state - height_min->state) / (height_max->state - height_min->state) * 100);
            } else if (message[0] == 0xe) {
                ESP_LOGV("jiecang_desk_sensor", "unit 0x%0X", message[2]);
                unit->publish_state(message[2]);
            } else if (message[0] == 0x20) {
                ESP_LOGV("jiecang_desk_sensor", "limits 0x%0X  max %i min %i", message[2], (message[2] & 1), (message[2]>>4));
                if ((message[2] & 1) == 0) { // low nibble 0 -> no max limit
                    height_max->publish_state(physical_max);
                }
                if ((message[2]>>4) == 0) { // high nibble 0 -> no min limit
                    height_min->publish_state(physical_min);
                }
            } else if (message[0] == 0x07) {
                ESP_LOGV("jiecang_desk_sensor", "physical limits 0x%02X%02X 0x%02X%02X", message[2], message[3], message[4], message[5]);
                physical_max = byte2float(message[2], message[3]);
                physical_min = byte2float(message[4], message[5]);
            } else if (message[0] == 0x21) {
                ESP_LOGV("jiecang_desk_sensor", "height_max 0x%02X%02X", message[2], message[3]);
                height_max->publish_state(byte2float(message[2], message[3]));
            } else if (message[0] == 0x22) {
                ESP_LOGV("jiecang_desk_sensor", "height_min 0x%02X%02X", message[2], message[3]);
                height_min->publish_state(byte2float(message[2], message[3]));
            } else if (message[0] == 0x25) {
                ESP_LOGV("jiecang_desk_sensor", "position1 0x%02X%02X", message[2], message[3]);
                position1->publish_state(byte2float(message[2], message[3]));
            } else if (message[0] == 0x26) {
                ESP_LOGV("jiecang_desk_sensor", "position2 0x%02X%02X", message[2], message[3]);
                position2->publish_state(byte2float(message[2], message[3]));
            } else if (message[0] == 0x27) {
                ESP_LOGV("jiecang_desk_sensor", "position3 0x%02X%02X", message[2], message[3]);
                position3->publish_state(byte2float(message[2], message[3]));
            } else if (message[0] == 0x28) {
                ESP_LOGV("jiecang_desk_sensor", "position4 0x%02X%02X", message[2], message[3]);
                position4->publish_state(byte2float(message[2], message[3]));
            } else {
                ESP_LOGV("jiecang_desk_sensor", "unknown message %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", message[0], message[1], message[2], message[3], message[4], message[5], message[6], message[7], message[8], message[9]);
            }
        }

        void update() override {
            const int max_length = 10;
            static unsigned int buffer[max_length];
            while (available()) {
                if(bufferMessage(read(), buffer, max_length)) {
                    handleMessage(buffer);
                }
            }
        }
};
