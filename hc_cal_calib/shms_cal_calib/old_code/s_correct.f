      function s_correct_pr(xhit,icol)
 
c     Correct for lateral coordinate of imapct point.
c     Fit to GEANT pion data @ 5 GeV/c (Simon).

      if((icol.ne.1).and.(icol.ne.2)) then
         print*,'*** s_correct_pr: icol=',icol,' ***'
         stop  '*** wrong icol in s_correct_pr ***'
      end if

c     Check hit coordinate with fired block number.
c     With Vardan's simulated data:
c      if((xhit.lt.0..and.icol.eq.1).or.(xhit.gt.0..and.icol.eq.2)) then
c     With Simon's simulated data:
      if((xhit.lt.0..and.icol.eq.2).or.(xhit.gt.0..and.icol.eq.1)) then
         x=xhit
      else
         x=0.
      end if
 
      if(x.lt.0.) x=-x
 
cc      s_correct_pr=1.0328+(x/73.395)**3.7036
cc      s_correct_pr=1.0328/s_correct_pr

cc      s_correct_pr=1.+(x/82.348)**2.537
cc      s_correct_pr=1./s_correct_pr

cc      s_correct_pr=1.+(x/91.51)**2.89
cc      s_correct_pr=1./s_correct_pr

      s_correct_pr=1.+(x/106.73)**2.329
      s_correct_pr=1./s_correct_pr

cD      print*,'s_correct_pr = ',s_correct_pr, '  x = ',x,
cD     *     '  xhit = ',xhit, '  icol = ',icol

      end

*=======================================================================
c
c      function s_correct_cal(xhit,iblk)
c 
cc     Correct for lateral coordinate of imapct point.
cc     GEANT muon data @ 1 GeV/c.
c
c      real xh(14),ef(14)
c      data xh/   0.,   5.,  10.,  15.,  20.,  25.,  30.,  35.,  40.,
c     *       45.,  50.,  55.,  60.,  65./
c      data ef/1.036,1.030,1.029,1.031,1.043,1.064,1.082,1.113,1.146,
c     *     1.192,1.271,1.360,1.497,1.697/
c      save xh,ef
c
c      parameter (xstp=5.)
c
cc     Check hit coordinate with fired block number.
c      if((xhit.lt.0.and.iblk.eq.1).or.(xhit.gt.0.).and.(iblk.eq.2)) then
c         x=xhit
c      else
c         x=0.
c      end if
c 
c      if(x.lt.0.) x=-x
c
c      i=x/xstp+1
c      if(i.lt.1) i=1
c      if(i.gt.14) i=14
c      eff=ef(i)+(ef(i+1)-ef(i))/(xh(i+1)-xh(i))*(x-xh(i))
c
c      s_correct_cal=ef(1)/eff
c      end

*=======================================================================

      function s_correct_sh(xh,yh,icol,irow)
      s_correct_sh=1.
      end
