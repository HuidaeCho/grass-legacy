      SUBROUTINE PUTT(ASTXT,POS,ASCHR)
      INCLUDE 'syspar.d'
C
C     PUT THE ASCII-CHAR INTO ASTXT AT POS
C
      INTEGER ASTXT(1), POS, ASCHR
C
      PARAMETER (MM=2**(ZCW*7)-1)
      PARAMETER (MC=2**7-1)
C
      IW = (POS+ZCW-1)/ZCW
      IC = (IW*ZCW) - POS
      ASTXT(IW) = IOR(IAND(ASTXT(IW),MM-ISHFT(MC,IC*7)),
     1   ISHFT( IAND(ASCHR,MC),IC*7))
      RETURN
      END
