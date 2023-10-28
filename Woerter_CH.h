// Woerter_CH
// Definition der schweizerischen Woerter fuer die Zeitansage.
// Die Woerter sind Bitmasken fuer die Matrix.

//   0 1 2 3 4 5 6 7 8 9 0
// 0 E S K I S C H A F U F
// 1 V I E R T U B F Z A H
// 2 Z W A N Z G S I V O R
// 3 A B O H A U B I E G E
// 4 E I S Z W O I S D R U
// 5 V I E R I F U F I Q T
// 6 S A C H S I S I B N I
// 7 A C H T I N U N I E L
// 8 Z A N I E R B E U F I
// 9 Z W O U F I N A G S I

#ifndef WOERTER_CH_H
#define WOERTER_CH_H

// Definition der Woerter
//------------------------------------------------------------------------------------------------------------
#define CH_VOR          matrix[2] |= 0b0000000011100000
#define CH_AB           matrix[3] |= 0b1100000000000000
#define CH_ESISCH       matrix[0] |= 0b1101111000000000
#define CH_H_GSI        matrix[9] |= 0b0000000011100000

#define CH_FUEF         matrix[0] |= 0b0000000011100000
#define CH_ZAEAE        matrix[1] |= 0b0000000011100000
#define CH_VIERTU       matrix[1] |= 0b1111110000000000
#define CH_ZWAENZG      matrix[2] |= 0b1111110000000000
#define CH_HAUBI        matrix[3] |= 0b0001111100000000

#define CH_H_EIS        matrix[4] |= 0b1110000000000000
#define CH_H_ZWOEI      matrix[4] |= 0b0001111000000000
#define CH_H_DRUE       matrix[4] |= 0b0000000011100000
#define CH_H_VIER       matrix[5] |= 0b1111100000000000
#define CH_H_FUEFI      matrix[5] |= 0b0000011110000000
#define CH_H_SAECHSI    matrix[6] |= 0b1111110000000000
#define CH_H_SIEBNI     matrix[6] |= 0b0000001111100000
#define CH_H_ACHTI      matrix[7] |= 0b1111100000000000
#define CH_H_NUENI      matrix[7] |= 0b0000011110000000
#define CH_H_ZAENI      matrix[8] |= 0b1111000000000000
#define CH_H_EUFI       matrix[8] |= 0b0000000111100000
#define CH_H_ZWOEUFI    matrix[9] |= 0b1111110000000000

#endif
