with Ada.Text_IO, Ada.Calendar, System.Multiprocessors;
use  Ada.Text_IO, Ada.Calendar, System.Multiprocessors;
procedure Time_PI is
  ---------------
  -- Constants --
  ---------------
    INTERVALS : constant Positive := 10_000_000;
  ---------------
  -- Protected --
  ---------------
    protected type Mutexed_Data is
        procedure Clear;
        function Get return Long_Long_Float;
        procedure Add(Amount : in Long_Long_Float);
      private
        Data : Long_Long_Float := 0.0;
      end Mutexed_Data;
    protected body Mutexed_Data is
        procedure Clear is begin Data := 0.0; end Clear;
        function Get return Long_Long_Float is begin return Data; end Get;
        procedure Add(Amount : in Long_Long_Float) is begin Data := Data + Amount; end Add;
      end Mutexed_Data;
  ---------------
  -- Variables --
  ---------------
    Previous : Time;
    Sum      : Mutexed_Data;
  -----------  
  -- Tasks --
  -----------
    task type Process is
      entry Initialize(Interval : in Positive; Total : in Positive);
      end Process;
    task body Process is
      I, T : Positive := 1;
      S : Long_Long_Float := 0.0;
      begin
        accept Initialize(Interval : in Positive; Total : in Positive) do I := Interval; T := Total; end Initialize;
        for J in T * (I - 1)..T * I - 1 loop
          S := S + 4.0 / (1.0 + (1.0 / Long_Long_Float(INTERVALS) * (Long_Long_Float(J) + 0.5))**2);
        end loop;
        Sum.Add(S);
      end Process;
  ----------
  -- Main --
  ----------
  begin
    New_Line;
    Put_Line("Rectangle rule PI calculator with" & Integer'image(INTERVALS) & " intervals");
    New_Line;
    for I in 1..Number_Of_CPUs loop
      Put_Line(CPU_Range'image(I) & " CPU(s):");
      Sum.Clear;
      Previous := Clock;
      Calculate : declare Processes : array(1..I) of Process; begin
        for J in Processes'range loop
          Processes(J).Initialize(Positive(J), INTERVALS / Positive(Processes'last));
        end loop;
      end Calculate;
      Put_Line("   Calculating PI takes" & Duration'image(Clock - Previous) & " seconds");
      Put_Line("   With the resulting PI of" & Long_Long_Float'image(Sum.Get * (1.0 / Long_Long_Float(INTERVALS))));
      New_Line;
    end loop;
  end Time_PI;
-- Sample output (run on home Windows desktop with AMD Quad):
--
-- C:\GNAT\2012\bin>gnatmake time_pi.adb -ada2012
-- gcc -c -ada2012 time_pi.adb
-- gnatbind -x time_pi.ali
-- gnatlink time_pi.ali
--
-- C:\GNAT\2012\bin>time_pi
--
-- Rectangle rule PI calculator with 10000000 intervals
--
-- 1 CPU(s):
--   Calculating PI takes 0.243275167 seconds
--   With the resulting PI of 3.14159265358979382E+00
--
-- 2 CPU(s):
--   Calculating PI takes 0.122008821 seconds
--   With the resulting PI of 3.14159265358979417E+00
--
-- 3 CPU(s):
--   Calculating PI takes 0.082370523 seconds
--   With the resulting PI of 3.14159245358978408E+00
--
-- 4 CPU(s):
--   Calculating PI takes 0.063102199 seconds
--   With the resulting PI of 3.14159265358979408E+
