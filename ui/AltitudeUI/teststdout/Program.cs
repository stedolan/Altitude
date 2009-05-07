using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace teststdout
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("[042] TEST MESSAGE");
            while (true)
            {
                Thread.Sleep(44000);
            }
        }
    }
}
