using Android.App;
using Android.Widget;
using Android.OS;
using Android.Support.Wearable.Activity;
using Android.Views;
using GalaSoft.MvvmLight.Helpers;
using Ondo.WearableApp.Model;

namespace Ondo.WearableApp
{
    [Activity(Label = "@string/app_name", MainLauncher = true)]
    public class MainActivity : WearableActivity
    {
        protected override async void OnCreate(Bundle bundle)
        {
            base.OnCreate(bundle);
            SetContentView(Resource.Layout.activity_main);

            SetAmbientEnabled();

            await Vm.GetData();
            AirconListView.Adapter = Vm.AirCons.GetAdapter(GetAirConView);
        }

        private View GetAirConView(int position, AirConDto aircon, View convertView)
        {
            View view = convertView ?? LayoutInflater.Inflate(Resource.Layout.aircons, null);
        
            var airconItem = view.FindViewById<CheckBox>(Resource.Id.AirconItem);

            airconItem.Checked = aircon.DevicePower;
            airconItem.Text = aircon.Location;
            airconItem.SetCommand("Click", Vm.ChangeStateOfAirCon, aircon);
            
            return view;
        }

        private DeviceViewModel Vm => ViewModelLocator.Instance.DeviceViewModel;

        private ListView AirconListView => FindViewById<ListView>(Resource.Id.AirconList);
    }
}


