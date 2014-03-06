-- autor: Roman Blanco
-- login: xblanc01
-- datum: 16.11.2013

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.std_logic_arith.all;
use IEEE.std_logic_unsigned.all;

entity ledc8x8 is
	port(
		SMCLK: in std_logic;
		ROW: out std_logic_vector (0 to 7);
		LED: out std_logic_vector (0 to 7);
		RESET: in std_logic
	);
end entity ledc8x8;

architecture signaly of ledc8x8 is
	signal ctrl_cnt: STD_LOGIC_VECTOR (21 downto 0);
	signal ce: STD_LOGIC;
	signal row_cnt: STD_LOGIC_VECTOR (0 to 7) := "10000000";
	signal switch: STD_LOGIC;
	signal radek: STD_LOGIC_VECTOR (0 to 7);
begin

divSignal: process (RESET, SMCLK)
begin
	if RESET = '1' then
		ctrl_cnt <= "0000000000000000000000";
	elsif SMCLK'event and SMCLK = '1' then
		ctrl_cnt <= ctrl_cnt + 1;
		if ctrl_cnt(7 downto 0) = "11111111" then 
			ce <= '1';
		else 
			ce <= '0';
		end if;
	end if;
	switch <= ctrl_cnt(21);
end process;

ctrlLine: process (RESET, SMCLK)
begin
	if RESET = '1' then
		row_cnt <= "10000000";
	elsif SMCLK'event and SMCLK = '1' then
		if ce = '1' then
			case row_cnt is
				when "10000000" => row_cnt <= "01000000";
				when "01000000" => row_cnt <= "00100000";
				when "00100000" => row_cnt <= "00010000";
				when "00010000" => row_cnt <= "00001000";
				when "00001000" => row_cnt <= "00000100";
				when "00000100" => row_cnt <= "00000010";
				when "00000010" => row_cnt <= "00000001";
				when "00000001" => row_cnt <= "10000000";
				when others => row_cnt <= "00000000";
			end case;
		end if;
	end if;
end process;

setLines: process (row_cnt)
begin
	case row_cnt is
		when "10000000" => radek <= "00011111";
		when "01000000" => radek <= "01011111";
		when "00100000" => radek <= "00111111";
		when "00010000" => radek <= "01010001";
		when "00001000" => radek <= "01010110";
		when "00000100" => radek <= "11110001";
		when "00000010" => radek <= "11110110";
		when "00000001" => radek <= "11110001";
		when others => radek <= "11111111";
	end case;
end process;

lightLines: process (row_cnt)
begin
	ROW <= row_cnt;
	if switch = '1' then
		LED <= radek;
	else 
		LED <= "11111111";
	end if;
end process;
end architecture signaly;

