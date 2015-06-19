*=======================================================================
      program shms_calib
*=======================================================================

c
c     SHMS calorimeter calibration with electrons.
c

*
      implicit none
*

c
      real thr_lo,thr_hi
      parameter (thr_lo=0.,thr_hi=1.E+8) !calibrate all the events.

      print*,'========================================================='
      print*,'Calibrating SHMS Calorimeter'

      call shms_clb_det(thr_lo,thr_hi)

      print*,'========================================================='

      end

*=======================================================================

	subroutine shms_clb_det(thr_lo,thr_hi)
	implicit none
c
	real thr_lo,thr_hi

        integer nrow_pr,ncol_sh,nrow_sh
cc        parameter ( nrow_pr=11,ncol_sh=14,nrow_sh=11)
        parameter ( nrow_pr=14,ncol_sh=14,nrow_sh=16)
	integer npmts
	parameter (npmts=2*nrow_pr+ncol_sh*nrow_sh)	!shms2
	integer npmts2
	parameter (npmts2=npmts*npmts)
	real*8 q0(npmts)
	real*8 qm(npmts,npmts)
	real*8 qe(npmts)
	real*8 q(npmts)
	real*8 eb
	real*8 e0
	real*8 ac(npmts)
	real*8 au(npmts)
	real*8 t
	real*8 s
	integer nev
	logical*1 eod
	integer i,j
	integer nf(npmts)
	integer minf
	parameter (minf=5) ! minimum number to hit pmt to include pmt in calib.
	integer nums(npmts)
	integer numsel
	real*8 q0s(npmts)
	real*8 qes(npmts)
	integer nsi,nsj
	real*8 acs(npmts)
	real*8 aus(npmts)
	real*8 aux(npmts2)
	integer jp
	integer spare_id
        parameter (spare_id=77)
	character*40 fn

	real xh

	do i=1,npmts
	   q0(i)=0.
	   qe(i)=0.
	   do j=1,npmts
	      qm(i,j)=0.
	   end do
	   au(i)=0.
	   ac(i)=0.
	   nf(i)=0
	end do
	e0=0.
c
	nev=0
	eod=.false.
	do while(.not.eod)
	   call s_get_data(eb,q,xh,eod,thr_lo,thr_hi)
	   if(.not.eod) then
	      do i=1,npmts
		 if(q(i).gt.0.) then
		    q0(i)=q0(i)+q(i)
		    qe(i)=qe(i)+eb*q(i)
                    if(i.eq.6) then 
ccD                       print*,'q0(6)=',q0(6)
ccD                       print*,'qe(6)=',qe(6)
ccD                       print*,'q(6)=',q(6)
                    end if
		    do j=1,npmts
		       qm(i,j)=qm(i,j)+q(i)*q(j)
ccD                       if(i.eq.6.and.j.eq.6) print*,'qm(6,6)=',qm(6,6)
		    end do
		    nf(i)=nf(i)+1
		 end if
	      end do
	      e0=e0+eb
	      nev=nev+1
	      if(nev/1000*1000.eq.nev) write(*,'(e10.3,i7)') e0,nev
	   end if
	end do

ccD        print*,'q0(6)=',q0(6)
ccD        print*,'qe(6)=',qe(6)
ccD        print*,'qm(6,6)=',qm(6,6)
ccD        pause

	do i=1,npmts
	   q0(i)=q0(i)/nev
	   qe(i)=qe(i)/nev
	   do j=1,npmts
	      qm(i,j)=qm(i,j)/nev
	   end do
	end do
	e0=e0/nev

c     Debug outputs.

        open(spare_id, file='qm.d')
        do j=1,npmts
           do i=1,npmts
              write(spare_id,*) qm(i,j),i,j
           end do
        end do
        close(spare_id)

        open(spare_id, file='q0.d')
        do i=1,npmts
           write(spare_id,*) q0(i),i
        end do
        close(spare_id)

        open(spare_id, file='qe.d')
        do i=1,npmts
           write(spare_id,*) qe(i),i
        end do
        close(spare_id)

	numsel=0
	do i=1,npmts
	   if(nf(i).ge.minf) then
	      numsel=numsel+1
	      nums(numsel)=i
ccD	      print*,nums(numsel),numsel,nf(i)
           else
	      write(*,*) ' PMT ',i,' only ',nf(i),
     *        ' events. Will not to be calibrated. Gain is set to 0.'
	   end if
	end do
ccD	print*,'numsel =',numsel

	write(*,'(''Total '',i6,'' events processed'')') nev
	write(*,*) ' PMT with less than', minf,
     *       ' events  are not included in calibration.'
	write(*,*)
	write(*,11) 'shms_neg_pre_hits=',(nf(i),i=        1,  nrow_pr)
	write(*,11) 'shms_pos_pre_hits=',(nf(i),i=nrow_pr+1,2*nrow_pr)
	write(*,11) 'shms_shower_hits =',
     *       (nf(i),i=2*nrow_pr+1,2*nrow_pr+nrow_sh)
        do j=2,ncol_sh
           write(*,11) '                  ',
     *          (nf(i),i=2*nrow_pr+(j-1)*nrow_sh+1,
     *          2*nrow_pr+j*nrow_sh)
        end do
c
	do i=1,numsel
	   nsi=nums(i)
	   q0s(i)=q0(nsi)
	   qes(i)=qe(nsi)
	   do j=1,numsel
	      nsj=nums(j)
	      jp=j+(i-1)*numsel
	      aux(jp)=qm(nsj,nsi)
ccD	      write(65,'(e12.5)') aux(jp)
	   end do
	end do

	call calib(e0,q0s,qes,aux,numsel,numsel*numsel,aus,acs)

	do i=1,numsel
	   nsi=nums(i)
	   au(nsi)=aus(i)
	   ac(nsi)=acs(i)
	end do

ccD	write(*,'(2e10.3,i5)') (ac(i),au(i),i,i=1,npmts)

        if(e0.lt.10.) then
           write(fn,'(i1,a6)') int(e0),'.param'
        else
           write(fn,'(i2,a6)') int(e0),'.param'
        end if

	open(spare_id,file=fn)

cc	write(spare_id,'(''; Calibration constants for SHMS momentum'',f5.1,
cc     *       '' GeV/c, '',i6,'' events processed'')') e0,nev
	write(spare_id,20) e0,nev
	write(spare_id,*)

        write(spare_id,10)
     &       'shms_neg_pre_gain=',(ac(i)*1.D+3,i=        1,  nrow_pr)
        write(spare_id,10)
     &       'shms_pos_pre_gain=',(ac(i)*1.D+3,i=nrow_pr+1,2*nrow_pr)
        write(spare_id,10) 'shms_shower_gain =',
     &       (ac(i)*1.D+3,i=2*nrow_pr+1,2*nrow_pr+nrow_sh)
        do j=2,ncol_sh
           write(spare_id,10) '                  ',
     &          (ac(i)*1.D+3,i=2*nrow_pr+(j-1)*nrow_sh+1,
     *          2*nrow_pr+j*nrow_sh)
        end do
	close(spare_id)

	write(*,20) e0,nev
	write(*,*)
        write(*,10)
     &       'shms_neg_pre_gain=',(ac(i)*1.D+3,i=        1,  nrow_pr)
        write(*,10)
     &       'shms_pos_pre_gain=',(ac(i)*1.D+3,i=nrow_pr+1,2*nrow_pr)
        write(*,10) 'shms_shower_gain =',
     &       (ac(i)*1.D+3,i=2*nrow_pr+1,2*nrow_pr+nrow_sh)
        do j=2,ncol_sh
           write(*,10) '                  ',
     &          (ac(i)*1.D+3,i=2*nrow_pr+(j-1)*nrow_sh+1,
     *          2*nrow_pr+j*nrow_sh)
        end do

 20     format('; Calibration constants for SHMS momentum',f5.1,
     *       ' GeV/c, ',i6,' events processed')
 10	format(a18,30(f6.3,','))
 11	format(a18,30(i5,','))

        open(spare_id,file='shms_calib.cal_data')
	write(*,*) 'Creating file s_cal_calib.cal_data, channel ',
     *  spare_id

        rewind(5)
	eod=.false.
	nev=0
	do while(.not.eod)
	   call s_get_data(eb,q,xh,eod,0.,1.E+8)
	   if(.not.eod) then
	      s=0.
*	      t=0.
	      do i=1,npmts
	         s=s+q(i)*ac(i)
*	         t=t+q(i)*au(i)
	      end do
              write(spare_id,*) s,eb,xh
	   end if
	end do

        close(5)
	close(spare_id)

	end

*=======================================================================

	subroutine calib(e0,q0,qe,aux,npmts,npmts2,au,ac)
	implicit none
	integer npmts,npmts2
	real*8 e0
	real*8 q0(npmts)
	real*8 qe(npmts)
	real*8 aux(npmts2)
	real*8 ac(npmts)
	real*8 au(npmts)
	real*8 qm(npmts,npmts)
	real*8 t
	real*8 s
	integer ifail
	integer i,j
	integer jp
c
	do i=1,npmts
	   do j=1,npmts
	      jp=j+(i-1)*npmts
	      qm(j,i)=aux(jp)
cD	      write(66,'(e12.5,2i4)') qm(j,i),j,i
	   end do
	end do
c
	print*,'Calib: npmts =',npmts
	print*,' '
c
	print*,'Inversing the Matrix...'
	call smxinv(qm,npmts,ifail)
	if(ifail.ne.0) then
	   stop '*** Singular Matrix ***'
	else
	   print*,'                    ...done.'
	end if
c
	do i=1,npmts
	   au(i)=0.
	   do j=1,npmts
	      au(i)=au(i)+qm(i,j)*qe(j)
	   end do
	end do
c
	s=0.
	do i=1,npmts
	   t=0.
	   do j=1,npmts
	      t=t+qm(i,j)*q0(j)
	   end do
	   s=s+q0(i)*t
	end do
c
	t=0.
	do i=1,npmts
	   t=t+au(i)*q0(i)
	end do
	s=(e0-t)/s
c
	do i=1,npmts
	   t=0.
	   do j=1,npmts
	      t=t+qm(i,j)*q0(j)
	   end do
	   ac(i)=s*t+au(i)
	end do
c
	end

*-----------------------------------------------------------------------

      subroutine s_get_data(eb,q,xh,eod,thr_lo,thr_hi)
      implicit none
c
      real*8 eb
      integer nrow_pr,ncol_sh,nrow_sh
cc      parameter (nrow_pr=11,ncol_sh=14,nrow_sh=11)
      parameter (nrow_pr=14,ncol_sh=14,nrow_sh=16)
      integer*4 num_pmts
      parameter (num_pmts=2*nrow_pr+ncol_sh*nrow_sh)	!shms2
      real*8 q(num_pmts)
      logical*1 eod

      integer*4 nhit 
      real*4 qdc
      integer*4 nh 
      integer*4 nb
      integer irow,icol,idet
c
      real*4 xh,yh, xph,yph
      real*4 s_correct_pr,s_correct_sh
      real*4 thr_lo,thr_hi
      logical*1 good_ev
      real*4 qnet

      real thr
      parameter (thr=15.)

      good_ev=.false.
      do while(.not.good_ev)

	 eb=0.d0
	 do nb=1,num_pmts
	    q(nb)=0.d0
	 end do
	 qnet=0.
	 eod=.true.

	 read(*,*,end=5) nhit,xh,yh,xph,yph,eb !x,y,xp,yp @ FP
cc	 read(*,*,end=5) nhit,yh,xh,eb
	 do nh=1,nhit

	    read(*,*,end=5) qdc,irow,icol,idet
            if(qdc.gt.thr) then

               if(idet.eq.1) then !preshower
                  nb=(icol-1)*nrow_pr+irow
                  q(nb)=qdc*s_correct_pr(xh+275.*xph,icol) !275cm - PrSwr dist.
ccD                  if(icol.eq.1.and.irow.eq.6) then
ccD                     print*,'get_data: qdc, q, nb=',qdc,q(nb),nb
ccD                     print*,'    xh, cor=',xh,s_correct_pr(xh,icol)
ccD                  end if
                  if(nb.eq.18) print*,'nb, xh, icol, cor = ',
     *                 nb, xh, icol, s_correct_pr(xh,icol)

               elseif(idet.eq.2) then !shower
                  nb=2*nrow_pr+(icol-1)*nrow_sh+irow
                  q(nb)=qdc*s_correct_sh(xh,yh,icol,irow)
               else
                  stop '*** wrong detector identifier ***'
               end if

               qnet=qnet+q(nb)
ccD     print*,qdc,idet,irow,icol,nb

            end if              !qdc>thr

	 enddo                  !1,nhit
	 eod=.false.

	 qnet=qnet/(eb*1000.)
	 good_ev=(qnet.gt.thr_lo).and.(qnet.lt.thr_hi)

cD	 write(99,*) qnet

      end do   !.not.good_ev

 5    continue

      end

*-----------------------------------------------------------------------

      SUBROUTINE SMXINV (A,NDIM,IFAIL)
C
C CERN PROGLIB# F107    SMXINV          .VERSION KERNFOR  1.0   720503
C ORIG. 03/05/72 CL
C
***      REAL*8 A(*),RI(100)
***      INTEGER*4 INDEX(100)
cc      REAL*8 A(*),RI(200)
cc      INTEGER*4 INDEX(200)
      REAL*8 A(*),RI(300)
      INTEGER*4 INDEX(300)
C
      DATA  TOL / 1.D-14/
C
      IFAIL=0
      N=NDIM
      NP1=N+1
      DO 10 I=1,N
   10 INDEX(I)=1
C
      DO 80 I=1,N
C
C--                FIND PIVOT
      PIVOT=0.0D0
      JJ=1
      DO 20 J=1,N
      IF (INDEX(J).EQ.0) GO TO 19
      ELM=DABS (A(JJ))
      IF (ELM.LE.PIVOT) GO TO 19
      PIVOT=ELM
      K=J
      KK=JJ
 19   JJ=JJ+NP1
 20   CONTINUE
      IF (PIVOT/DABS(A(1)).LT.TOL) GO TO 100
      INDEX(K)=0
      PIVOT=-A(KK)
C
C--                ELIMINATION
      KJ=K
      NP=N
C
      DO 70 J=1,N
      IF (J-K) 34,30,34
C
 30   A(KJ)=1.0D0/PIVOT
      RI(J)=0.0D0
      NP=1
      GO TO 70
C
   34 ELM=-A(KJ)
   40 RI(J)=ELM/PIVOT
      IF (ELM.EQ.0.0D0) GO TO 50
C
      JL=J
      DO 45 L=1,J
      A(JL)=A(JL)+ELM*RI(L)
 45   JL=JL+N
C
 50   A(KJ)=RI(J)
C
 70   KJ=KJ+NP
C
 80   CONTINUE
C
C--                CHANGE THE SIGN AND PROVISIONAL FILL-UP
      IJ0=1
      JI0=1
      DO 95 I=1,N
      IJ=IJ0
      JI=JI0
C
      DO 90 J=1,I
      A(IJ)=-A(IJ)
      A(JI)=A(IJ)
      IJ=IJ+N
      JI=JI+1
 90   CONTINUE
C
      IJ0=IJ0+1
      JI0=JI0+N
 95   CONTINUE
      RETURN
C
C--                FAILURE RETURN
 100  IFAIL=1
      RETURN
      END

*=======================================================================
	include 's_correct.f'

