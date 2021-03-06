﻿namespace Ondo.Backend.Core.Model
{
    public class AirCon
    {
        public string Id { get; set; }

        public string Location { get; set; }

        public bool DevicePower { get; set; }

        public bool QuietOn { get; set; }

        public bool PowerfulOn { get; set; }

        public int FanMode { get; set; }

        public bool SwingVOn { get; set; }

        public bool SwingHOn { get; set; }

        public int TargetTempC { get; set; }

        public bool IsConnected { get; set; }
    }
}
