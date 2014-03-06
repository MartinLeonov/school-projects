-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2013 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Zdenek Vasicek <vasicek AT fit.vutbr.cz>
--

library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru


   -- synchronni pamet ROM
   CODE_ADDR : out std_logic_vector(11 downto 0); -- adresa do pameti
   CODE_DATA : in std_logic_vector(7 downto 0);   -- CODE_DATA <- rom[CODE_ADDR] pokud CODE_EN='1'
   CODE_EN   : out std_logic;                     -- povoleni cinnosti

   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(9 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (1) / zapis (0)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti

   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data

   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic                       -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -- zde dopiste potrebne deklarace signalu

   signal PCounter_reg:            std_logic_vector (11 downto 0);
   signal PCounter_dec:            std_logic;
   signal PCounter_inc:            std_logic;
   signal PCBus_allow:             std_logic;

   signal Instruction_reg:         std_logic_vector (7 downto 0);
   signal Instruction_reg_load:    std_logic := '0';
   signal Instruction_reg_decoded: type Instruction_type;
   type Instruction_type is (instIncVal, instDecVal, instIncPtr,
   instDecPtr, instPrint, instExit, instNop);

   signal MPointer_reg:            std_logic_vector (9 downto 0);
   signal MPointer_dec:            std_logic;
   signal MPointer_inc:            std_logic;
   signal MPointerBus_allow:       std_logic;

   signal Ignore_save:             std_logic := '0';

   signal Counter_reg:             std_logic_vector (3 downto 0);
   signal Counter_dec:             std_logic;
   signal Counter_inc:             std_logic;

   signal PState:                  type State; := stInit;
   signal NState:                  type State

   type State: is (stInit, stPrint, stPrint2, stDecode, stIdle, stFetch0,
   stFetch1, stExit, stIncPtr, stDecPtr, stIncVal, stIncVal2, stDecVal,
   stDecVal2);

begin
   FSM:process(OUT_BUSY)
   begin

 -- zde dopiste vlastni VHDL kod


   program_counter: process(CLK, RESET, PCounter_dec, PCounter_inc)
   begin
      if (RESET = '1') then
         PCounter_reg <= "00000000000";
      elsif (CLK'event and CLK = '1') then
         if (PCounter_dec = '1' and PCounter_inc = '0') then
            PCounter_reg <= PCounter_reg - 1;
         elsif (PCounter_dec = '0' and PCounter_inc = '1') then
            PCounter_reg <= PCounter_reg + 1;
         end if;
      end if;
   end process;

   CODE_ADDR <= PCounter_reg when PCBus_allow = '1' else
      (others => 'Z');


   instruction_register: process(CLK, RESET, Instruction_reg_load)
   begin
      if (RESET = '1') then
         Instruction_reg <= "0000000";
      elsif (CLK'event and CLK = '1') then
         if (Instruction_reg_load = '1') then
            Instruction_reg <= CODE_DATA;
         end if;
      end if;
   end process;


   instruction_decoder: process(Instruction_reg)
   begin
      case Instruction_reg is
         when X"2B" => Instruction_reg_decoded <= instIncVal; -- +
         when X"2D" => Instruction_reg_decoded <= instDecVal; -- -
         when X"3E" => Instruction_reg_decoded <= instIncPtr; -- >
         when X"3C" => Instruction_reg_decoded <= instDecPtr; -- <
         when X"5B" => Instruction_reg_decoded <= instWhileL; -- [
         when X"5D" => Instruction_reg_decoded <= instWhileR; -- ]
         when X"2C" => Instruction_reg_decoded <= instRead;   -- ,
         when X"2E" => Instruction_reg_decoded <= instPrint;  -- .
         when X"00" => Instruction_reg_decoded <= instExit;   --
         when others => Instruction_reg_decoded <= instNop;   --
      end case;
   end process;


   memory_pointer: process(CLK, RESET, Pointer_dec, Pointer_inc)
   begin
      if (RESET = '1') then
         Pointer_reg <= "000000000";
      elsif (CLK'event and CLK = '1') then
         if (Pointer_dec = '1' and Pointer_inc = '0') then
            Pointer_reg = Pointer_reg - 1;
         elsif (Pointer_dec = '0' and Pointer_inc = '1') then
            Pointer_reg = Pointer_reg + 1;
         end if;
      end if;
   end process;


   DATA_ADDR <= MPointer_reg when MPointerBus_allow = '1' else
      (others => 'Z');


   counter: process(CLK, RESET)
   begin
      if (RESET = '1') then
         Counter_reg <= "00000000000";
      elsif (CLK'event and CLK = '1') then
         if (Counter_dec = '1' and Counter_inc = '0') then
            Counter_reg = Counter_reg - 1;
         elsif (Counter_dec = '0' and Counter_inc = '1') then
            Counter_reg = Counter_reg + 1;
         end if;
      end if;
   end process;


   present_state: process(EN, CLK)
   begin
      if (EN = '1') then
         if (CLK'event and CLK = '1') then
            if (RESET = '1') then
               PState <= STInit;
            else
               PState <= Nstate;
            end if;
         end if;
      end if;
   end process;

   fsm: process(OUT_BUSY, DATA_RDATA, IN_VLD, Instruction_reg_decoded, Ignore_save)
   begin
     Counter_reg <= '0';
     PState <= '0';
     Pointer_reg <= '0';
     Instruction_reg <= '0';
     PCounter_reg <= '0';
     CODE_EN <= '0';
     stDecode <= '0';
     NState <= '0';
     Instruction_reg_load <= '0';
     PCBus_allow <= '0';

     case Pstate is

        when stInit =>
           NState <= stFetch0;

        when Fetch0 =>
           NState <= stFetch1;
           PCBus_allow <= '1';
           CODE_EN <= '1';

        when Fetch1 =>
           Instruction_reg_load <= '1';
           NState <= stDecode;

        when stDecode =>

           case Instruction_reg is

              when instExit =>
                 NState <= stExit;

              when instIncPtr =>
                 NState <= stIncPtr;

              when instDecPtr =>
                 NState <= stDecPtr;

              when instIncVal =>
                 NState <= stIncVal;

              when instDecVal =>
                 NState <= stDecVal;

              when instNop =>
                 NState <= stFetch0;

              when instPrint =>
                 NState => stPrint;

        when stExit =>
           NState <= stExit;

        when stIncVal =>
           DATA_EN <= '1';
           MPointerBus_allow <= '1';
           NState <= sfetch0;
           DATA_RDWR = '1';

           NState <= stIncVal2;

        when stIncIncVal2 =>
           PTR_allowBus <= '1';
           DATA_EN <= '1';
           DATA_RDWR <= '0';
           DATA_WDATA <= DATA_RDATA + 1;

           PCounter_dec <= '0';
           PCounter_inc <= '1';

           NState <= stFetch0;

        when stDecVal =>
           DATA_EN <= '1';
           MPointerBus_allow <= '1';
           DATA_RDWR <= '1';

           NState <= stDecVal2;

        when stDecVal2 =>
           MPointerBus_allow <= '1';
           DATA_EN <= '1';
           DATA_RDWR <= '0';
           DATA_WDATA <= DATA_RDATA - 1;

           PCounter_dec <= '1';
           PCounter_inc <= '0';

           NState <= stFetch0;

        when stIncPtr =>
           MPointer_inc <= '1';
           MPointer_dec <= '0';

           PCounter_inc <= '1';
           PCounter_dec <= '0';

           NState <= stFetch0;

        when stDecPtr =>
           MPointer_inc <= '0';
           MPointer_dec <= '1';

           PCounter_inc <= '0';
           PCounter_dec <= '1';

           NState <= stFetch0;

        when stPrint =>
           DATA_EN <= '1';
           MPointerBus_allow <= '1';
           DATA_RDWR <= '1';

           NState <= stPrint2;

        when stPrint2 =>
           if (OUT_BUSY = '0') then
              OUT_WE <= '1';
              OUT_DATA <= DATA_RDATA;

              PCounter_dec <= '0';
              PCounter_inc <= '1';
              NState <= stFetch0;
           else NState <= stPrint2;
           end if;

        when others =>
           NState <= stFetch0;
     end case;
   end process;
end behavioral;
