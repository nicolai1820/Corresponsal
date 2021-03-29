package co.com.netcom.corresponsal;

import android.app.AlertDialog;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.ingenico.lar.bc.PinpadCallbacks;

import java.util.Arrays;
import java.util.List;

import static com.ingenico.lar.bc.Pinpad.PP_OK;

public class CardDialog {

    public static class Builder {
        private AlertDialog dialog;
        private AlertDialog.Builder dialogBuilder;
        private OptionsAdapter adapter;
        private LayoutInflater inflater;
        private PinpadCallbacks.MenuResult menuResult;

        public Builder(Context context, String[] options, PinpadCallbacks.MenuResult menuResult) {
            this(context, Arrays.asList(options), menuResult);
        }

        Builder(Context context, List<String> options, PinpadCallbacks.MenuResult menuResult){

            this.menuResult = menuResult;

            inflater = LayoutInflater.from(context);
            View view = inflater.inflate(R.layout.menu_dialog, null);
            RecyclerView rvMenuOptions = view.findViewById(R.id.rvMenuOptions);
            LinearLayoutManager linearLayoutManager = new LinearLayoutManager(context);

            adapter = new OptionsAdapter(options);

            dialogBuilder = new AlertDialog.Builder(context);
            dialogBuilder.setView(view);
            dialogBuilder.setTitle("SELECCIONE UNA OPCION");


            rvMenuOptions.setAdapter(adapter);
            rvMenuOptions.setLayoutManager(linearLayoutManager);

        }

        public Builder setTitle(final String title) {
            dialogBuilder.setTitle(title);
            return this;
        }

        public void show() {
            dialogBuilder.create();
            dialog = dialogBuilder.show();
        }


        private class OptionsAdapter extends RecyclerView.Adapter<OptionsAdapter.OptionViewHolder> {

            List<String> list;

            OptionsAdapter(List<String> options) {
                list = options;
            }

            @Override
            public OptionViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
                View view = inflater.inflate(R.layout.dialog_item, parent, false);

                return new OptionViewHolder(view);
            }

            @Override
            public void onBindViewHolder(final OptionViewHolder holder, final int position) {
                holder.tvOption.setText(list.get(position));
                holder.tvOption.setOnClickListener(new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        dialog.dismiss();
                        menuResult.setResult(PP_OK, holder.getAdapterPosition());
                    }
                });
            }

            @Override
            public int getItemCount() {
                return list.size();
            }

            class OptionViewHolder extends RecyclerView.ViewHolder {
                TextView tvOption;

                OptionViewHolder(View itemView) {
                    super(itemView);
                    tvOption = itemView.findViewById(R.id.tvOption);
                }
            }
        }
    }
}
