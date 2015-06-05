library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

entity time_stopclock_generator is
   port (
      CLK : in std_logic;
      CLK_1Hz : in std_logic;
      time_enable : in std_logic;
      time_reset : in std_logic;
      hour_sclk_0 : inout integer range 0 to 9;
      hour_sclk_1 : inout integer range 0 to 9;
      minute_sclk_0 : inout integer range 0 to 9;
      minute_sclk_1 : inout integer range 0 to 9;
      second_sclk_0 : inout integer range 0 to 9;
      second_sclk_1 : inout integer range 0 to 9
   );
end entity;

-- popis architektury stopek
architecture stopclock of time_stopclock_generator is
begin
   process(CLK)
   begin
      if (time_enable = '1') then
         if (CLK'event) and (CLK = '1') then
            if (CLK_1Hz = '1') then
               if (second_sclk_0 = 5) and (second_sclk_1 = 9) then
                  if (minute_sclk_0 = 5) and (minute_sclk_1 = 9) then
                     if (hour_sclk_0 = 2) and (hour_sclk_1 = 3) then
                        -- 23:59:59
                        hour_sclk_0 <= 0;
                        hour_sclk_1 <= 0;
                        minute_sclk_0 <= 0;
                        minute_sclk_1 <= 0;
                        second_sclk_0 <= 0;
                        second_sclk_1 <= 0;
                     elsif (hour_sclk_1 = 9) then
                        -- X9:59:59
                        hour_sclk_0 <= hour_sclk_0 + 1;
                        hour_sclk_1 <= 0;
                     else
                        -- XX:59:59
                        hour_sclk_1 <= hour_sclk_1 + 1;
                        minute_sclk_0 <= 0;
                        minute_sclk_1 <= 0;
                        second_sclk_0 <= 0;
                        second_sclk_1 <= 0;
                     end if;
                  elsif (minute_sclk_1 = 9) then
                     -- XX:X9:59
                     minute_sclk_0 <= minute_sclk_0 + 1;
                     minute_sclk_1 <= 0;
                  else
                     -- XX:XX:59
                     minute_sclk_1 <= minute_sclk_1 + 1;
                     second_sclk_0 <= 0;
                     second_sclk_1 <= 0;
                  end if;
               elsif (second_sclk_1 = 9) then
                  -- XX:XX:X9
                  second_sclk_0 <= second_sclk_0 + 1;
                  second_sclk_1 <= 0;
               else
                  -- XX:XX:XX
                  second_sclk_1 <= second_sclk_1 + 1;
               end if;
            end if;
         end if;
      end if;
      if (time_reset = '1') then
         hour_sclk_0 <= 0;
         hour_sclk_1 <= 0;
         minute_sclk_0 <= 0;
         minute_sclk_1 <= 0;
         second_sclk_0 <= 0;
         second_sclk_1 <= 0;
      end if;
   end process;
end stopclock;
