      program tofcal
! Fit TOF for Hall C HMS or SOS with the form for each PMT:
! tcorr = time - offset - path * velocity - adccor / sqrt(ADC)
! where offset, velocity, and adccor are parameters
! April 10, 2005 P. Bosted
! Input file: tofcal.inp should contain name of data file to be fit
!             First xter of file name should be "h" for HMS data
!             and "s" for SOS. 
! To generate an input file, set xdumptof = 1 in xdebug.param in
! the PARAM directory, where x=h for HMS, x=s for SOS
! The HMS output will be in fort.37, and the SOS in fort.38
! The output parameters will be in tofcal.param. Insert these
! into the xhodo.param file that you are using. Make sure to 
! erase the old values of sigma, or put the new ones after the
! old ones, so that they get used properly
! Normal values of invadc_offset are between -50 and 50,
! normal values of invadc_velocity are between 12 and 17 (50 is
! default if not enough data for the fit), and normal values of
! shodo_pos_invadc_adc are 20 to 50. Normal values of the sigmas
! are 0.3 to 0.8 nsec. 

      implicit none
      integer i,j,k,n,nparam,ipn,ipl,idt,idet(1000),nhit(200)
      integer ip1(200),ip2(200),ip3(200),ifail,nhit0(200)
      integer ipdet(600),iptyp(600),iwork(1000),k1,k2,k3,k4,k5,k6
      real*8 tr(1000),p(1000),zc(1000),tc1(1000),p2(1000),hival,dtc
      real*8 ax(1000,1000),bx(1000),sum1(200),sum2(200),sum3(200)
      real*8 mean0(200),sig0(200),mean(200),sig(200),dt,adc(1000)
      real*8 toff(200),vel(200),quad(200),chisq,df,ttol,adcmin(200)
      real*8 avtime,ntime
      integer thist(200,10),iloop,adchist(200,18),phist(200,18)
      integer thistf(200,50),adchistf(200,50)
      integer adchistpm(200,20,2)

      character*80 string
      character*40 filename

      open(unit=7,file='tofcal.inp')
      read(7,'(a)') filename
      open(unit=8,file='tofcal.out')
      open(unit=11,file='tofcal.parampass1')
      open(unit=12,file='tofcal.param')
      open(unit=13,file='tofcal.adchist')
      write(13,'(1x,'' xv'',50f5.0)') (20.*(j-0.5),j=1,50)
      open(unit=14,file='tofcal.tdchist')
      write(14,'(1x,'' xv'',50f5.1)') (-5.0+0.2*(j-0.5),j=1,50)

! first see how many hits per PMT: need at least 100 for fitting
! also make ADC histograms
      open(unit=9,file=filename,err=998)
       if(filename(1:1).ne.'h'.and.filename(1:1).ne.'s') then
        write(6,'(1x,''error, input file name '',
     >   '' sould start with h or s'')')
        goto 999
       endif
       write(*,*) ' filling adc histograms'
       do i=1,10000000
        n=0
        do j=1,1000
          read(9,'(a)',end=10) string
          read(string,'(1x,i1)') k
          if(k.eq.0) goto 9
          n=n+1
          read(string,'(1x,i1,2i3,5f10.3)') ipn,ipl,idt, 
     >      tr(n),p(n),zc(n),tc1(n),adc(n)
          k = min(18, max(1, int((adc(n)/20.)+1)))
          idet(n) = 100*(ipn-1) + 20*(ipl-1) + idt
          adchist(idet(n), k) = adchist(idet(n), k) + 1 
          k = min(50, max(1, int((adc(n)/20.)+1)))
          adchistf(idet(n), k) = adchistf(idet(n), k) + 1 
          nhit0(idet(n)) = nhit0(idet(n)) + 1.
        enddo
 9      continue
      enddo

! find more or less where threshold is
 10   do i=1,200
        adcmin(i)=50.
        if(nhit0(i).gt.0.) then
          write(13,'(i3,50i4)') i,(min(999,adchistf(i,j)),j=1,50)
          hival=0.
          k=0
          do j=1,17
            if(adchist(i,j).gt.hival) then
              hival = adchist(i,j)
              k = j
            endif
          enddo
c for test
c         do n=1,max(1,k)
c           if(adchist(i,n).lt.hival/20.) adcmin(i)=20.*(n-0.5)
c         enddo
c         write(13,'(1x,''adcmin='',f6.1)') adcmin(i) 
        endif
      enddo
      close(unit=9)
      do i=1,200
        sum1(i)=0.
        sum2(i)=0.
        sum3(i)=0.
      enddo

! Do everything twice, 2nd time with tighter tolerance (ttol) on
! time differences, based on first pass
      ttol=20.0
      do iloop=1,2
         write(*,*) ' loop = ',iloop
! Initialize the fitting arrays
        do i=1,1000
          if(i.le.200) nhit(i)=0
          bx(i)=0.
          do j=1,1000
            ax(i,j)=0.
          enddo
        enddo

! read in data
      open(unit=9,file=filename)
      nparam=0
      chisq=0.
      df=0.
      do i=1,10000000
        n=0
        do j=1,1000
          read(9,'(a)',end=12) string
          read(string,'(1x,i1)') k
          if(k.eq.0) goto 11
          n=n+1
          read(string,'(1x,i1,2i3,5f10.3)') ipn,ipl,idt, 
     >      tr(n),p(n),zc(n),tc1(n),adc(n)
! linearize the detector numbers
          idet(n) = 100*(ipn-1) + 20*(ipl-1) + idt
          if(idet(n).lt.1.or.idet(n).gt.200) write(6,'(1x,
     >      ''error! '',4i4)') ipn,ipl,idt,idet(n)

! correct raw times for zpos using betap
          tr(n) = tr(n)- zc(n)

! remember path**2 or sqrt(p) or adc
          p(n) = min(200., max(0., p(n)))
c          adc(n) = min(500, max(0., adc(n)))
          p2(n) = 1./sqrt(max(20., adc(n)))
          k = min(18, max(1, int(p(n)/7.)+1))
          phist(idet(n), k) = phist(idet(n), k) + 1 
! ignore hits from paddles that are rarely hit
          if(nhit0(idet(n)).lt.100) n=n-1
        enddo

! Loop over all pairs, if at least 6
 11     if(n.ge.6) then
! see if this is first time a detector is used
          do j=1,n
            nhit(idet(j))=nhit(idet(j))+1

! If first time detector used, assign corresponding parameters
! Note that detector had has a fixed time offset (ip1) of zero
! since all times are relative)
            if(nhit(idet(j)).eq.1) then
              if(idet(j).eq.10) then
                ip1(idet(j))=0
              else

! fixed time offsets
                nparam=nparam+1
                ip1(idet(j))=nparam
                ipdet(nparam)=idet(j)
                iptyp(nparam)=1
              endif

! linear term in path 
              nparam=nparam+1
              ip2(idet(j))=nparam
              ipdet(nparam)=idet(j)
              iptyp(nparam)=2

! quadratic term for path**2
              nparam=nparam+1
              ip3(idet(j))=nparam
              ipdet(nparam)=idet(j)
              iptyp(nparam)=3
              k=idet(j)
              write(8,'(1x,i3,4i5)') k,nhit(k),ip1(k),ip2(k),ip3(k)
            endif
          enddo ! loop over n

! now loop over all pairs in fill in the matrix
! also accumulate sums to get means and sigmas for each detector
! of original corrected times as read in from the file
          do j=1,n-1
            do k=j+1,n
              dt = tc1(j)-tc1(k)
              dtc = dt
              if(iloop.eq.1.or.abs(dt).lt.ttol) then
               idt = min(10,max(1,int((dt+5.))+1))
               thist(idet(j),idt) = thist(idet(j),idt) + 1
               chisq = chisq + dt**2
               df = df + 1
               sum1(idet(j)) = sum1(idet(j)) + 1.
               sum2(idet(j)) = sum2(idet(j)) + dt
               sum3(idet(j)) = sum3(idet(j)) + dt**2
               dt = tc1(k)-tc1(j)
               idt = min(10,max(1,int((dt+5.))+1))
               thist(idet(k),idt) = thist(idet(k),idt) + 1
               sum1(idet(k)) = sum1(idet(k)) + 1.
               sum2(idet(k)) = sum2(idet(k)) + dt
               sum3(idet(k)) = sum3(idet(k)) + dt**2
              endif
              k1 = idet(j)
              k2 = idet(k)
              dt = (tr(j) + toff(k1) + vel(k1)*p(j) + quad(k1)*p2(j))-
     >             (tr(k) + toff(k2) + vel(k2)*p(k) + quad(k2)*p2(k))
              if(iloop.eq.1.or.abs(dt).lt.ttol) then
               k1=ip1(idet(j))
               k2=ip1(idet(k))
               k3=ip2(idet(j))
               k4=ip2(idet(k))
               k5=ip3(idet(j))
               k6=ip3(idet(k))
               if(k1.gt.0) then
                bx(k1) = bx(k1) - (tr(j)-tr(k))
                ax(k1,k1) = ax(k1,k1) + 1.
                ax(k1,k3) = ax(k1,k3) + p(j)
                ax(k1,k4) = ax(k1,k4) - p(k)
                ax(k1,k5) = ax(k1,k5) + p2(j)
                ax(k1,k6) = ax(k1,k6) - p2(k)
                ax(k3,k1) = ax(k3,k1) + p(j)
                ax(k4,k1) = ax(k4,k1) - p(k)
                ax(k5,k1) = ax(k5,k1) + p2(j)
                ax(k6,k1) = ax(k6,k1) - p2(k)
               endif
               if(k1.gt.0.and.k2.gt.0) then
                ax(k1,k2) = ax(k1,k2) - 1.
                ax(k2,k1) = ax(k2,k1) - 1.
               endif
               if(k2.gt.0) then
                bx(k2) = bx(k2) + (tr(j)-tr(k))
                ax(k2,k2) = ax(k2,k2) + 1.
                ax(k2,k3) = ax(k2,k3) - p(j)
                ax(k2,k4) = ax(k2,k4) + p(k)
                ax(k2,k5) = ax(k2,k5) - p2(j)
                ax(k2,k6) = ax(k2,k6) + p2(k)
                ax(k3,k2) = ax(k3,k2) - p(j)    
                ax(k4,k2) = ax(k4,k2) + p(k)    
                ax(k5,k2) = ax(k5,k2) - p2(j)    
                ax(k6,k2) = ax(k6,k2) + p2(k)    
               endif
               bx(k3) = bx(k3) - (tr(j)-tr(k)) * p(j)
               bx(k4) = bx(k4) + (tr(j)-tr(k)) * p(k)
               bx(k5) = bx(k5) - (tr(j)-tr(k)) * p2(j)
               bx(k6) = bx(k6) + (tr(j)-tr(k)) * p2(k)
               ax(k3,k3) = ax(k3,k3) + p(j)*p(j)
               ax(k3,k4) = ax(k3,k4) - p(k)*p(j)
               ax(k3,k5) = ax(k3,k5) + p2(j)*p(j)
               ax(k3,k6) = ax(k3,k6) - p2(k)*p(j)
               ax(k4,k3) = ax(k4,k3) - p(j)*p(k)
               ax(k4,k4) = ax(k4,k4) + p(k)*p(k)
               ax(k4,k5) = ax(k4,k5) - p2(j)*p(k)
               ax(k4,k6) = ax(k4,k6) + p2(k)*p(k)
               ax(k5,k3) = ax(k5,k3) + p(j)*p2(j)
               ax(k5,k4) = ax(k5,k4) - p(k)*p2(j)
               ax(k5,k5) = ax(k5,k5) + p2(j)*p2(j)
               ax(k5,k6) = ax(k5,k6) - p2(k)*p2(j)
               ax(k6,k3) = ax(k6,k3) - p(j)*p2(k)
               ax(k6,k4) = ax(k6,k4) + p(k)*p2(k)
               ax(k6,k5) = ax(k6,k5) - p2(j)*p2(k)
               ax(k6,k6) = ax(k6,k6) + p2(k)*p2(k)
              endif
            enddo
          enddo
        endif ! if n>1
      enddo ! loop over reading in data 
 12   close(unit=9)

! Histograms
      if(iloop.eq.1) then
       do i=1,200
        if(sum1(i).gt.0.) then
          write(13,'(i3,18i4)') i,(phist(i,j),j=1,18)
        endif
       enddo
      endif
! see how many hits per detector and get mean, sigma of original corr. times
      do i=1,200
        if(sum1(i).gt.0.) then
          mean0(i)=sum2(i)/sum1(i)
          sig0(i)=sqrt(sum3(i)/sum1(i) - mean0(i)**2)
        endif
        write(8,'(1x,i3,i5,2f6.2,10i5)') i,nhit(i),
     >    mean0(i),sig0(i),(min(9999,thist(i,j)),j=1,10)
        sum1(i)=0.
        sum2(i)=0.
        sum3(i)=0.
        do j=1,10
          thist(i,j)=0
        enddo
      enddo
      df = max(1., df - nparam)
      write(6,'(1x,''initial chi2/d.f.='',f8.3,'' for '',f7.0,
     >   '' deg. freedom'')') chisq/df,df
      write(8,'(1x,''initial chi2/d.f.='',f8.3,'' for '',f7.0,
     >   '' deg. freedom'')') chisq/df,df

! find the solutions
      call deqn (nparam,ax,1000,iwork,ifail,1,bx)
      write(8,'(1x,''ifail='',i10)') ifail

! association of parameters with detectors
      do i=1,nparam
        if(iptyp(i).eq.1) toff(ipdet(i))=bx(i)
        if(iptyp(i).eq.2)  vel(ipdet(i))=bx(i)
        if(iptyp(i).eq.3) quad(ipdet(i))=bx(i)
      enddo

! write solutions
      write(10 + iloop,'(/''; use '',a,
     >  ''tofusinginvadc=1 if want invadc_offset''/
     >  '';  invadc_linear, and invadc_adc to be used''/
     >            a,''tofusinginvadc=1'')') 
     >  filename(1:1),filename(1:1)
      write(10+iloop,'(/a,''hodo_pos_invadc_offset ='',3(f8.2,'',''),
     >  f8.2)') filename(1:1),(-1.0*toff(i),i=1,80,20)
      do j=2,16
       write(10+iloop,'(1x,''                        '',3(f8.2,'',''),
     >  f8.2)')(-1.0*toff(i),i=j,79+j,20)
      enddo

      write(10+iloop,'(/a,''hodo_neg_invadc_offset ='',3(f8.2,'',''),
     >  f8.2)')filename(1:1),(-1.0*toff(i),i=101,180,20)
      do j=2,16
       write(10+iloop,'(1x,''                        '',3(f8.2,'',''),
     >  f8.2)')(-1.0*toff(i),i=100+j,179+j,20)
      enddo

      write(10+iloop,'(/a,''hodo_pos_invadc_linear ='',3(f8.2,'',''),
     >  f8.2)')filename(1:1),( -1./min(-1./15.,vel(i)),i=1,80,20)
      do j=2,16
       write(10+iloop,'(1x,''                        '',3(f8.2,'',''),
     >  f8.2)')(-1./min(-1./15.,vel(i)),i=j,79+j,20)
      enddo

      write(10+iloop,'(/a,''hodo_neg_invadc_linear ='',3(f8.2,'',''),
     >  f8.2)')filename(1:1),( -1./min(-1./15.,vel(i)),i=101,180,20)
      do j=2,16
       write(10+iloop,'(1x,''                        '',3(f8.2,'',''),
     >  f8.2)')(-1./min(-1./15.,vel(i)),i=100+j,179+j,20)
      enddo

      write(10+iloop,'(/a,''hodo_pos_invadc_adc='',3(f9.2,'',''),
     >  f9.2)')filename(1:1),(-1.*quad(i),i=1,80,20)
      do j=2,16
       write(10+iloop,'(1x,''                   '',3(f9.2,'',''),
     >  f9.2)')(-1.*quad(i),i=j,79+j,20)
      enddo

      write(10+iloop,'(/a,''hodo_neg_invadc_adc='',3(f9.2,'',''),
     >  f9.2)')filename(1:1),(-1.0*quad(i),i=101,180,20)
      do j=2,16
       write(10+iloop,'(1x,''                   '',3(f9.2,'',''),
     >  f9.2)')(-1.*quad(i),i=100+j,179+j,20)
      enddo


! read in data again and look at sigmas relative to average fp time
      chisq = 0.
      df = 0.
      open(unit=9,file=filename)
      do i=1,10000000
        n=0
        do j=1,1000
          read(9,'(a)',end=14) string
          read(string,'(1x,i1)') k
          if(k.eq.0) goto 13
          n=n+1
          read(string,'(1x,i1,2i3,5f10.3)') ipn,ipl,idt, 
     >      tr(n),p(n),zc(n),tc1(n),adc(n)
          idet(n) = 100*(ipn-1) + 20*(ipl-1) + idt
          tr(n) = tr(n)- zc(n)
          p(n) = min(200., max(0., p(n)))
c          adc(n) = min(500, max(0., adc(n)))
          p2(n) = 1./sqrt(max(20., adc(n)))
! ignore hits from paddles that are rarely hit
          k1 = idet(n)
          dt = (tr(n) + toff(k1) + vel(k1)*p(n) + quad(k1)*p2(n))  
          if(nhit0(idet(n)).lt.100) n=n-1
        enddo
! Loop over all pairs, if at least 6 hits
 13     if(n.ge.6) then
          do j=1,n
            avtime=0.
            ntime=0.
            do k=1,n
             if(k.ne.j) then
              k1 = idet(j)
              k2 = idet(k)
              dt = (tr(k) + toff(k2) + vel(k2)*p(k) + quad(k2)*p2(k))
              avtime = avtime + dt
              ntime = ntime + 1.
             endif
            enddo
            avtime = avtime / ntime
            dt = (tr(j) + toff(k1) + vel(k1)*p(j) + quad(k1)*p2(j))-
     >             avtime
            if(iloop.eq.2) then
              idt = min(50,max(1,int((dt+ 5.)/0.2)+1))
              thistf(idet(j),idt) = thistf(idet(j),idt) + 1
              idt = max(1,min(20, int(adc(j)/50.)+1))
              if(dt.gt.0.) adchistpm(idet(j),idt,1)=
     >          adchistpm(idet(j),idt,1)+1
              if(dt.le.0.) adchistpm(idet(j),idt,2)=
     >          adchistpm(idet(j),idt,2)+1
            endif
            if(iloop.eq.1.or.abs(dt).lt.ttol) then
              idt = min(10,max(1,int((dt+ 5.))+1))
              thist(idet(j),idt) = thist(idet(j),idt) + 1
              chisq = chisq + dt**2
              df = df + 1
              sum1(idet(j)) = sum1(idet(j)) + 1.
              sum2(idet(j)) = sum2(idet(j)) + dt
              sum3(idet(j)) = sum3(idet(j)) + dt**2
            endif
          enddo
        endif ! if n>5
      enddo ! loop over reading in data 
 14   close(unit=9)
! see how many hits per detector and get mean, sigma of original corr. times
      do i=1,200
        sig(i)=100.
        if(sum1(i).gt.0.) then
          mean(i)=sum2(i)/sum1(i)
          sig(i)=sqrt(sum3(i)/sum1(i) - mean(i)**2)
        endif
        write(8,'(1x,i3,i5,2f6.2,10i5)') i,nhit(i),
     >    mean(i),sig(i),(min(9999,thist(i,j)),j=1,10)
        if(iloop.eq.2)
     >    write(14,'(1x,i3,50i5)') i,(min(9999,thistf(i,j)),j=1,50)
        sum1(i)=0.
        sum2(i)=0.
        sum3(i)=0.
        do j=1,10
          thist(i,j)=0
        enddo
      enddo
      df = max(1., df - nparam)
      write(6,'(1x,''  final chi2/d.f.='',f8.3,'' for '',f7.0,
     >   '' deg. freedom'')') chisq/df,df
      write(8,'(1x,''  final chi2/d.f.='',f8.3,'' for '',f7.0,
     >   '' deg. freedom'')') chisq/df,df

      write(10+iloop,'(/a,''hodo_pos_sigma ='',3(f8.2,'',''),
     >  f8.2)')filename(1:1),(sig(i),i=  1, 80,20)
      do j=2,16
       write(10+iloop,'(1x,''                '',3(f8.2,'',''),
     >  f8.2)')(sig(i),i=    j, 79+j,20)
      enddo
      write(10+iloop,'(/a,''hodo_neg_sigma ='',3(f8.2,'',''),
     >  f8.2)')filename(1:1),(sig(i),i=101,180,20)
      do j=2,16
       write(10+iloop,'(1x,''                '',3(f8.2,'',''),
     >  f8.2)')(sig(i),i=100+j,179+j,20)
      enddo

      enddo ! loop over time tolerance

      do i=1,200
        write(15,'(1x,i3,20i4)') i,(adchistpm(i,j,1),j=1,20)
        write(15,'(1x,i3,20i4)') i,(adchistpm(i,j,2),j=1,20)
      enddo

      goto 999

 998  write(6,'(1x,''error, cant find file '',a)') filename

 999  stop
      end

