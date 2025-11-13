#ifndef MOTOR_H
#define MOTOR_H

void Motor_Init(void);
void Motor_Passo_Completo(int passo);
void Motor_Meio_Passo(int passo);
void Motor_Girar(int passos, int sentido, int modo);

#endif
