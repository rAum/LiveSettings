using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace VarSync
{
    class Variable
    {
        public string Type { get; set; } 
        public string Name { get; set; }
        public string Value { get; set; }

        public override string ToString()
        {
            return String.Format("{0} {1} = {2}\n", Type, Name, Value);
        }
    }
}
