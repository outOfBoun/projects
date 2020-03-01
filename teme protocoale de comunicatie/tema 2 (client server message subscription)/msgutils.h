//Voicu Alex-Georgian

struct Subscription
{
    char subscribe; //sub / unsub
    char topic[51];
    char sf; //1 / 0
};

int tcpsend(int sock, char *buff, int size);
int tcprecv(int sock, char *buff, int buff_size, int expected_size);
