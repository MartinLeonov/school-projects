library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

entity time_clock_generator is
   port (
      CLK : in std_logic;
      CLK_1Hz : in std_logic;
      time_set : in std_logic;
      hour_clk_0 : inout integer range 0 to 9;
      hour_clk_1 : inout integer range 0 to 9;
      minute_clk_0 : inout integer range 0 to 9;
      minute_clk_1 : inout integer range 0 to 9;
      second_clk_0 : inout integer range 0 to 9;
      second_clk_1 : inout integer range 0 to 9
   );
end entity;

-- popis architektury hodin
architecture clock of time_clock_generator is
begin
   process(CLK)
   begin
     if (CLK'event) and (CLK = '1') then
        if (CLK_1Hz = '1') then
           if (second_clk_0 = 5) and (second_clk_1 = 9) then
              if (minute_clk_0 = 5) and (minute_clk_1 = 9) then
                 if (hour_clk_0 = 2) and (hour_clk_1 = 3) then
                    -- 23:59:59
                    hour_clk_0 <= 0;
                    hour_clk_1 <= 0;
                    minute_clk_0 <= 0;
                    minute_clk_1 <= 0;
                    second_clk_0 <= 0;
                    second_clk_1 <= 0;
                 elsif (hour_clk_1 = 9) then
                    -- X9:59:59
                    hour_clk_0 <= hour_clk_0 + 1;
                    hour_clk_1 <= 0;
                 else
                    -- XX:59:59
                    hour_clk_1 <= hour_clk_1 + 1;
                    minute_clk_0 <= 0;
                    minute_clk_1 <= 0;
                    second_clk_0 <= 0;
                    second_clk_1 <= 0;
                 end if;
              elsif (minute_clk_1 = 9) then
                 -- XX:X9:59
                 minute_clk_0 <= minute_clk_0 + 1;
                 minute_clk_1 <= 0;
              else
                 -- XX:XX:59
                 minute_clk_1 <= minute_clk_1 + 1;
                 second_clk_0 <= 0;
                 second_clk_1 <= 0;
              end if;
           elsif (second_clk_1 = 9) then
              -- XX:XX:X9
              second_clk_0 <= second_clk_0 + 1;
              second_clk_1 <= 0;
           else
              -- XX:XX:XX
              second_clk_1 <= second_clk_1 + 1;
           end if;
        end if;
     end if;
   end process;
end clock;
