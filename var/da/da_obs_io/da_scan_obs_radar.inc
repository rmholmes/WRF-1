subroutine da_scan_obs_radar (iv, filename, grid)

   !---------------------------------------------------------------------------
   ! Purpose: Scan the radar observation file
   !---------------------------------------------------------------------------

   implicit none

   type (iv_type),    intent(inout) :: iv
   character(len=*),  intent(in)    :: filename
   type(domain),     intent(in)     :: grid     ! first guess state.

   integer                       :: i, j, n, iost, nlevels, fm
   integer                       :: total_radar
   integer                       :: iunit

   type (radar_multi_level_type) :: platform

   character (LEN = 120)         :: char_total_radar
   character (LEN = 120)         :: char_ned

   logical                       :: outside, outside_all
   integer                       :: n_dup, ndup

   real*8                        :: obs_time, analysis_time
   integer                       :: iyear, imonth, iday, ihour, imin
   real                          :: tdiff, dlat_earth,dlon_earth,crit
   integer                       :: itt,itx,iout
   logical                       :: iuse, thin_3d
   integer                       :: i1,j1,k
   real                          :: dx,dy,dxm,dym,zk
   real                          :: v_h(kms:kme)


   if (trace_use) call da_trace_entry("da_scan_obs_radar")
   thin_3d=.false. 
   if( thin_conv) thin_3d=.true.
   ! 1. open file
   ! ============

   call da_get_unit(iunit)
   open(unit   = iunit,     &
        FILE   = trim(filename), &
        FORM   = 'FORMATTED',  &
        ACCESS = 'SEQUENTIAL', &
        iostat =  iost,     &
        STATUS = 'OLD')

   if (iost /= 0) then
      ! Does not matter of radar file missing
      call da_warning(__FILE__,__LINE__, &
         (/"Cannot open radar file "//filename/))
      call da_free_unit(iunit) 
      if (trace_use) call da_trace_exit("da_scan_obs_radar")
      return
   end if
   ! 1.1  Initialize
   ! ============


   ! 2. read total radar
   ! ===================

   ! 2.1 read first line
   !     ---------------

   read (unit=iunit, fmt = '(A)', iostat = iost) char_total_radar
   if (iost /= 0) then
      ! Does matter if present and unreadable
      call da_error(__FILE__,__LINE__, &
         (/"Cannot read radar file"/))
   end if

   ! 2.3 total radar number

   read (unit=char_total_radar (15:17),fmt='(I3)', iostat = iost) total_radar

   ! 2.4 skip one lines

   read (unit=iunit, fmt = '(A)', iostat = iost)

   ! 3. read radar data

   do n = 1, total_radar

   if ( thin_conv ) call cleangrids_conv(radar)
      ! 3.1 skip one blank line

      read (unit=iunit, fmt = '(A)', iostat = iost)

      ! 3.2 read header

      read (unit=iunit, fmt = '(A)', iostat = iost) char_ned

      ! 3.3 read header information

      read (unit=char_ned (69:74), fmt='(I6)', iostat = iost) platform % stn % numobs

      ! 3.4 skip two lines

      read (unit=iunit, fmt = '(A)', iostat = iost)
      read (unit=iunit, fmt = '(A)', iostat = iost)

      ! 3.5 loop over records

      reports: do j = 1, platform % stn % numobs

         ! 3.5.1 read station general info

         read (unit = iunit, iostat = iost, &
                      fmt = '(A12,3X,A19,2X,2(F12.3,2X),F8.1,2X,I6)') &
                      platform % info % platform,  &
                      platform % info % date_char, &
                      platform % info % lat,       &
                      platform % info % lon,       &
                      platform % info % elv,       &
                      platform % info % levels

         if (platform%info%lon == 180.0  ) platform%info%lon =-180.000
         ! Fix funny wind direction at Poles
         if (platform%info%lat < -89.9999 .or. platform%info%lat > 89.9999) then
            platform%info%lon = 0.0
         end if

         read(unit=platform % info % platform (4:6), fmt='(I3)') fm

         !     3.5.2 read each level

         do i = 1, platform % info % levels
            ! height
            platform%each (i) = radar_each_level_type(missing_r, missing, -1.0,&
               field_type(missing_r, missing, missing_r, missing, missing_r), & ! rv
               field_type(missing_r, missing, missing_r, missing, missing_r))   ! rf

            read (unit = iunit, fmt = '(3X, F12.1, 2(F12.3,I4,F12.3,2X))') &
                             platform % each (i) % height,           &
                             platform % each (i) % rv % inv,         &
                             platform % each (i) % rv % qc,          &
                             platform % each (i) % rv % error,       &
                             platform % each (i) % rf % inv,         &
                             platform % each (i) % rf % qc,          &
                             platform % each (i) % rf % error

            if (platform % each (i) % rv % error == 0.0) then
                 platform % each (i) % rv % error  = 1.0
            end if

            if (platform % each (i) % rf % error == 0.0) then
                 platform % each (i) % rf % error  = 1.0
            end if

            if (platform % each (i) % rv % inv   == missing_r .or. &
                platform % each (i) % rv % error == missing_r) then
                platform % each (i) % rv % qc     = missing_data
            end if

            if (platform % each (i) % rf % inv   == missing_r .or. &
                platform % each (i) % rf % error == missing_r) then
                platform % each (i) % rf % qc     = missing_data
            end if

         end do

         ! Check if outside of the time range:

         read (platform%info%date_char,'(i4,4(1x,i2))') &
               iyear, imonth, iday, ihour, imin
         call da_get_julian_time (iyear,imonth,iday,ihour,imin,obs_time)
         if ( obs_time < time_slots(0) .or. &
              obs_time >= time_slots(num_fgat_time) ) then
            cycle reports
         endif

         call da_llxy (platform%info, platform%loc, outside, outside_all)
         if( outside_all ) cycle reports

         read (analysis_date,'(i4,4(1x,i2))')iyear, imonth, iday, ihour, imin
         call da_get_julian_time (iyear,imonth,iday,ihour,imin,analysis_time)
         tdiff = abs((obs_time - analysis_time)-0.02)
         dlat_earth = platform%info%lat
         dlon_earth = platform%info%lon
         if (dlon_earth < 0.0) dlon_earth = dlon_earth + 360.0
         if (dlon_earth >= 360.0) dlon_earth = dlon_earth - 360.0
         dlat_earth = dlat_earth * deg2rad
         dlon_earth = dlon_earth * deg2rad

         nlevels = platform%info%levels

         if (nlevels > max_ob_levels) then
             write(unit=message(1),fmt='(A,2I8)') &
                ' radar=> nlevels > max_ob_levels:',nlevels, max_ob_levels
             call da_warning(__FILE__,__LINE__,message(1:1))

             nlevels = max_ob_levels
             platform%info%levels = nlevels
         else if (nlevels < 1) then
            cycle reports
         end if


         ! Loop over duplicating obs for global
         n_dup = 1
         if (global .and. (platform%loc%i == ids .or. platform%loc%i == ide)) n_dup= 2
   
         do ndup = 1, n_dup
            select case (fm)

            case (128)
               if (.not.use_radarobs .or. iv%info(radar)%ntotal == max_radar_input) cycle reports
               if ( .not. thin_conv ) then
                  if (ndup==1) iv%info(radar)%ntotal = iv%info(radar)%ntotal + 1
                  if (outside) cycle reports
                  iv%info(radar)%nlocal = iv%info(radar)%nlocal + 1
               else 
                  crit = tdiff
                  i1   = platform%loc%i
                  j1   = platform%loc%j
                  dx   = platform%loc%dx
                  dy   = platform%loc%dy
                  dxm  = platform%loc%dxm
                  dym  = platform%loc%dym

                  do k=kms,kme
                     v_h(k) = dym*(dxm*grid%xb%h(i1,j1,k)+dx*grid%xb%h(i1+1,j1,k)) + &
                              dy*(dxm*grid%xb%h(i1,j1+1,k)+dx*grid%xb%h(i1+1,j1+1,k))
                  end do

                  do k=1,nlevels
                     if (ndup==1) iv%info(radar)%ntotal = iv%info(radar)%ntotal + 1
                     if (outside) cycle 
                     call da_to_zk(platform%each(k)%height, v_h, v_interp_h, zk)
                     if ( zk == missing_r ) cycle   
                     call map2grids_conv(radar,dlat_earth,dlon_earth,crit,iv%info(radar)%nlocal,itx,1,itt,iout,iuse,zk,thin_3d)
                     if ( .not. iuse ) cycle   
                  end do 
               end if ! if thin_conv

            case default;
               write(unit=stdout, fmt='(a)') 'Warning: unsaved obs found:'

               write(unit=stdout, fmt='(2a)') &
                  'platform % info % platform=', platform % info % platform

               write(unit=stdout, fmt='(a, i3)') &
                  'platform % info % levels=', platform % info % levels
            end select

            if( .not. thin_conv) then
               iv%info(radar)%max_lev = max(iv%info(radar)%max_lev, platform%info%levels)
            else
               iv%info(radar)%max_lev = 1
            end if
         end do        !  loop over duplicate
      end do reports
      
   end do ! total_radar

   close (iunit)
   call da_free_unit(iunit)

   if (trace_use) call da_trace_exit("da_scan_obs_radar")

end subroutine da_scan_obs_radar