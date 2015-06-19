      program check_cor

      do i=0,70,1
         x=i*1.
         print*, s_correct_pr(x,1),x
      enddo

      stop
      end

      include 's_correct.f'
