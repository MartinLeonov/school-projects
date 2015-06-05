library ieee;
use ieee.std_logic_1164.all;

entity fsm is
   port (
      CLK : in std_logic;
      KEY : in std_logic_vector(15 downto 0);
      ENABLE : out std_logic;
      OUTPUT : out std_logic;
      RESET : out std_logic
   );
end fsm;

architecture main of fsm is

   type time_state is (time_stop, time_pause, time_running, time_clock);
   signal state_present, state_next : time_state;

begin

   sync_logic: process(CLK)
   begin
      if (CLK'EVENT) and (CLK = '1') then
         state_present <= state_next;
      end if;
   end process sync_logic;

   next_state_logic: process(state_present, KEY)
   begin
      case (state_present) is
         when (time_running) =>
            if (KEY = "1000000000000000") then state_next <= time_pause;
            elsif (KEY = "0100000000000000") then state_next <= time_stop;
            elsif (KEY = "0010000000000000") then state_next <= time_clock;
            elsif (KEY = "0000000000000000") then state_next <= time_running;
            else state_next <= time_running;
            end if;
         when (time_pause) =>
            if (KEY = "1000000000000000") then state_next <= time_running;
            elsif (KEY = "0100000000000000") then state_next <= time_stop;
            elsif (KEY = "0010000000000000") then state_next <= time_clock;
            elsif (KEY = "0000000000000000") then state_next <= time_pause;
            else state_next <= time_running;
            end if;
         when (time_stop) =>
            if (KEY = "0010000000000000") then state_next <= time_clock;
            else state_next <= time_pause;
            end if;
         when (time_clock) =>
            if (KEY = "1000000000000000") then state_next <= time_pause;
            elsif (KEY = "0100000000000000") then state_next <= time_stop;
            elsif (KEY = "0010000000000000") then state_next <= time_clock;
            elsif (KEY = "0000000000000000") then state_next <= time_running;
            else state_next <= time_running;
            end if;
         when others =>
            state_next <= time_pause;
      end case;
   end process next_state_logic;

   output_logic: process(state_present, KEY)
   begin
      case (state_present) is
         when (time_running) =>
            ENABLE <= '1';
            RESET <= '0';
            OUTPUT <= '0';
         when (time_stop) =>
            ENABLE <= '0';
            RESET <= '1';
            OUTPUT <= '0';
         when (time_pause) =>
            ENABLE <= '0';
            RESET <= '0';
            OUTPUT <= '0';
         when (time_clock) =>
            ENABLE <= '0';
            RESET <= '0';
            OUTPUT <= '1';
         when others =>
            ENABLE <= '0';
            RESET <= '0';
            OUTPUT <= '1';
      end case;
   end process output_logic;
end main;
