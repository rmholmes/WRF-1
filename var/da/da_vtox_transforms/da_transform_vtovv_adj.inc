subroutine da_transform_vtovv_adj(grid, cv_size, be, cv, vv)

   !-----------------------------------------------------------------------
   ! Purpose: TBD
   !-----------------------------------------------------------------------

   implicit none

   type(domain),  intent(inout) :: grid
   integer,       intent(in)    :: cv_size ! Size of cv array.
   type(be_type), intent(in)    :: be   ! Background error structure.
   real,          intent(inout) :: cv(cv_size)   ! control variables.
   type(vp_type), intent(inout) :: vv   ! Grid point/EOF control var.

   integer :: s(4)   ! Index bounds into arrays.
   integer :: n      ! Loop counter.
   integer :: mz     ! Vertical truncation.
   integer :: ne     ! Ensemble size.

   logical :: scaling
 
   if (trace_use) call da_trace_entry("da_transform_vtovv_adj")

   if( .not. use_rf .or. do_normalize ) s(1:2)=1

   !-------------------------------------------------------------------------
   ! [2.0] Perform VToVV Transform:
   !-------------------------------------------------------------------------

   ! [2.1] Transform 1st control variable:
   mz = be % v1 % mz
   s(3)=s(1)+mz-1
   if( do_normalize )call da_transform_rescale(mz,be%sd(:,:,s(1):s(3)),vv%v1)
   if( use_rf .and. mz > 0 .and. len_scaling1(1) /= 0.0) then
      call da_transform_through_rf_adj(grid, mz, be % v1 % rf_alpha, be % v1 % val, vv % v1)
   elseif( mz > 0 ) then
      s(4)=s(2)+nij(0,0,2)*nij(0,1,2)*mz-1
      call da_transform_through_wavelet_adj(grid,mz,be%wsd(:,:,s(1):s(3)),cv(s(2):s(4)),vv%v1)
      s(2)=s(4)+1
   else
      print'(a,": be%v1%mz=",I0)',__FILE__,mz
   endif
   s(1)=s(3)+1

   ! [2.2] Transform 2nd control variable:

   mz = be % v2 % mz
   s(3)=s(1)+mz-1
   if( do_normalize )call da_transform_rescale(mz,be%sd(:,:,s(1):s(3)),vv%v2)
   if( use_rf .and. mz > 0 .and. len_scaling2(1) /= 0.0) then
      call da_transform_through_rf_adj(grid, mz, be % v2 % rf_alpha, be % v2 % val, vv % v2)
   elseif( mz > 0 ) then
      s(4)=s(2)+nij(0,0,2)*nij(0,1,2)*mz-1
      call da_transform_through_wavelet_adj(grid,mz,be%wsd(:,:,s(1):s(3)),cv(s(2):s(4)),vv%v2)
      s(2)=s(4)+1
   else
      print'(a,": be%v2%mz=",I0)',__FILE__,mz
   endif
   s(1)=s(3)+1

   ! [2.3] Transform 3rd control variable

   mz = be % v3 % mz
   s(3)=s(1)+mz-1
   if( do_normalize )call da_transform_rescale(mz,be%sd(:,:,s(1):s(3)),vv%v3)
   if( use_rf .and. mz > 0 .and. len_scaling3(1) /= 0.0) then
      call da_transform_through_rf_adj(grid, mz, be % v3 % rf_alpha, be % v3 % val, vv % v3)
   elseif( mz > 0 ) then
      s(4)=s(2)+nij(0,0,2)*nij(0,1,2)*mz-1
      call da_transform_through_wavelet_adj(grid,mz,be%wsd(:,:,s(1):s(3)),cv(s(2):s(4)),vv%v3)
      s(2)=s(4)+1
   else
      print'(a,": be%v3%mz=",I0)',__FILE__,mz
   endif
   s(1)=s(3)+1
   
   ! [2.4] Transform 4th control variable
      
   mz = be % v4 % mz
   s(3)=s(1)+mz-1
   if( do_normalize )call da_transform_rescale(mz,be%sd(:,:,s(1):s(3)),vv%v4)
   if( use_rf .and. mz > 0 .and. len_scaling4(1) /= 0.0) then
      call da_transform_through_rf_adj(grid, mz, be % v4 % rf_alpha, be % v4 % val, vv % v4)
   elseif( mz > 0 ) then
      s(4)=s(2)+nij(0,0,2)*nij(0,1,2)*mz-1
      call da_transform_through_wavelet_adj(grid,mz,be%wsd(:,:,s(1):s(3)),cv(s(2):s(4)),vv%v4)
      s(2)=s(4)+1
   else
      print'(a,": be%v4%mz=",I0)',__FILE__,mz
   endif
   s(1)=s(3)+1

   ! [2.5] Transform 5th control variable

   mz = be % v5 % mz
   s(3)=s(1)+mz-1
   if( do_normalize )call da_transform_rescale(mz,be%sd(:,:,s(1):s(3)),vv%v5)
   if( use_rf .and. mz > 0 .and. len_scaling5(1) /= 0.0) then
      call da_transform_through_rf_adj(grid, mz, be % v5 % rf_alpha, be % v5 % val, vv % v5)
   elseif( mz > 0 ) then
      s(4)=s(2)+nij(0,0,2)*nij(0,1,2)*mz-1
      call da_transform_through_wavelet_adj(grid,mz,be%wsd(:,:,s(1):s(3)),cv(s(2):s(4)),vv%v5)
      s(2)=s(4)+1
   else
      print'(a,": be%v5%mz=",I0)',__FILE__,mz
   endif
   s(1)=s(3)+1

#ifdef CLOUD_CV
   ! [2.6] Transform 6th control variable
   if (cloud_cv_options == 3)then
      scaling = .true.
   else
      scaling = .false.
   endif
   mz = be % v6 % mz
   if( use_rf .and. mz > 0 .and. len_scaling6(1) /= 0.0) then
      if(cloud_cv_options == 1)then
         vv % v6 = 0.0 
      elseif(cloud_cv_options == 2)then
         call da_transform_through_rf_adj(grid, mz, be % v6 % rf_alpha, be % v6 % val, vv % v6)
      elseif(cloud_cv_options == 3)then
         call da_transform_through_rf_adj(grid, mz, be % v6 % rf_alpha, be % v6 % val, vv % v6, scaling)
!        call da_transform_through_rf2_adj(grid, mz, be % v6 % rf_alpha, be % v6 % val, vv % v6)
      endif
   elseif( .not. use_rf ) then
      call da_error(__FILE__,__LINE__,(/"no da_transform_through_wavelet_adj for v6"/))
   endif

   ! [2.7] Transform 7th control variable

   mz = be % v7 % mz
   if( use_rf .and. mz > 0 .and. len_scaling7(1) /= 0.0) then
      if(cloud_cv_options == 1)then
         vv % v7 = 0.0
      elseif(cloud_cv_options == 2)then
         call da_transform_through_rf_adj(grid, mz, be % v7 % rf_alpha, be % v7 % val, vv % v7)
      elseif(cloud_cv_options == 3)then
         call da_transform_through_rf_adj(grid, mz, be % v7 % rf_alpha, be % v7 % val, vv % v7, scaling)
!        call da_transform_through_rf2_adj(grid, mz, be % v7 % rf_alpha, be % v7 % val, vv % v7)
      endif
   elseif( .not. use_rf ) then
      call da_error(__FILE__,__LINE__,(/"no da_transform_through_wavelet_adj for v7"/))
   endif

   ! [2.8] Transform 8th control variable

   mz = be % v8 % mz
   if( use_rf .and. mz > 0 .and. len_scaling8(1) /= 0.0) then
      if(cloud_cv_options == 1)then
         vv % v8 = 0.0
      elseif(cloud_cv_options == 2)then
         call da_transform_through_rf_adj(grid, mz, be % v8 % rf_alpha, be % v8 % val, vv % v8)
      elseif(cloud_cv_options == 3)then
         call da_transform_through_rf_adj(grid, mz, be % v8 % rf_alpha, be % v8 % val, vv % v8, scaling)
!        call da_transform_through_rf2_adj(grid, mz, be % v8 % rf_alpha, be % v8 % val, vv % v8)
      endif
   elseif( .not. use_rf ) then
      call da_error(__FILE__,__LINE__,(/"no da_transform_through_wavelet_adj for v8"/))
   endif

   ! [2.9] Transform 9th control variable

   mz = be % v9 % mz
   if( use_rf .and. mz > 0 .and. len_scaling9(1) /= 0.0) then
      if(cloud_cv_options == 1)then
         vv % v9 = 0.0
      elseif(cloud_cv_options == 2)then
         call da_transform_through_rf_adj(grid, mz, be % v9 % rf_alpha, be % v9 % val, vv % v9)
      elseif(cloud_cv_options == 3)then
         call da_transform_through_rf_adj(grid, mz, be % v9 % rf_alpha, be % v9 % val, vv % v9, scaling)
      endif
   elseif( .not. use_rf ) then
      call da_error(__FILE__,__LINE__,(/"no da_transform_through_wavelet_adj for v9"/))
   endif
#endif

   ! [2.9] Transform alpha control variable

   ne = be % ne
   if (ne > 0) then
      mz = be % alpha % mz
      if( do_normalize )then
         do n = 1, ne
            call da_transform_rescale(mz,be%alpha%sd,vv%alpha(:,:,:,n))
         end do
      endif
      if( use_rf )then
         do n = 1, ne
            call da_transform_through_rf_adj(grid, mz, be % alpha % rf_alpha, be % alpha % val, vv % alpha(:,:,:,n))
         end do
      else
         do n = 1, ne
            s(4)=s(2)+nij(0,0,2)*nij(0,1,2)*mz-1
            call da_transform_through_wavelet_adj(grid,mz,be%alpha%wsd,cv(s(2):s(4)),vv%alpha(:,:,:,n))
            s(2)=s(4)+1
         end do
      endif
   endif

   if( use_rf )then
      !-------------------------------------------------------------------------
      ! [1.0] Fill 1D cv array from 3-dimensional vv arrays.
      !-------------------------------------------------------------------------
!     cv=0.0
#ifdef CLOUD_CV
      call da_vv_to_cv( vv, grid%xp, (/ be%v1%mz, be%v2%mz, be%v3%mz, be%v4%mz, be%v5%mz,  be%v6%mz, be%v7%mz, be%v8%mz,be%v9%mz, be%alpha%mz, be%ne /), &
                        cv_size, cv)
#else
      call da_vv_to_cv( vv, grid%xp, (/ be%v1%mz, be%v2%mz, be%v3%mz, be%v4%mz, be%v5%mz, be%alpha%mz, be%ne /), cv_size, cv)
#endif
   endif

   if (trace_use) call da_trace_exit("da_transform_vtovv_adj")

endsubroutine da_transform_vtovv_adj