#include "exogiri.h"

void
Exogiri_error_array_pusher(void *ctx, xmlErrorPtr error)
{
  Errors* errors = (Errors*)ctx;
  ERL_NIF_TERM new_error;
  ERL_NIF_TERM existing_errors;
  existing_errors = errors->errors;
  new_error = enif_make_string(errors->env, error->message, ERL_NIF_LATIN1);
  errors->errors = enif_make_list_cell(
    errors->env,
    new_error,
    existing_errors
  );
}