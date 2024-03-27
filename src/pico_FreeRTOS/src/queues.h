#ifndef QUEUES_H
#define QUEUES_H

struct message
{
    double latitude;
    double longitude;
    double speed;
    int message_type;
};

struct gps
{
    double latitude;
    double longitude;
};

void initQueues();
int message_enqueue(struct message x);
int message_queue_dequeue(struct message* x);
void gps_queue_overwrite(struct gps x);
int gps_queue_peek(struct gps* x);
void vehicle_speed_queue_overwrite(uint8_t speed);
int vehicle_speed_queue_peek(uint8_t *speed);

#endif
