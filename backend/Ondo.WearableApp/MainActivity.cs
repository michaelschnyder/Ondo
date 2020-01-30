using Android.App;
using Android.Widget;
using Android.OS;
using Android.Support.Wearable.Activity;
using Ondo.WearableApp.Model;
using Ondo.WearableApp.ViewModel;
using System.Collections.Generic;

namespace Ondo.WearableApp
{
    [Activity(Label = "@string/app_name", MainLauncher = true)]
    public class MainActivity : WearableActivity
    {
        List<AirConDto> viewCollection = new List<AirConDto>();
        protected override async void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView(Resource.Layout.activity_main);

            SetAmbientEnabled();

            await Vm.GetData();
            


            LoadData();

            ListView lst = FindViewById<ListView>(Resource.Id.AirconList);
            lst.Adapter = new ListViewAdapter(this, viewCollection, Vm);
        }

        private void LoadData()
        {
            foreach (var aircon in Vm.AirCons)
            {
                viewCollection.Add(aircon);
            }
        }

        private DeviceViewModel Vm => ViewModelLocator.Instance.DeviceViewModel;
    }
}


