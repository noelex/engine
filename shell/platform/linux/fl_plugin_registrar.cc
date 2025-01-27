// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "flutter/shell/platform/linux/public/flutter_linux/fl_plugin_registrar.h"
#include "flutter/shell/platform/linux/fl_plugin_registrar_private.h"
#include "flutter/shell/platform/linux/fl_view_private.h"

#include <gmodule.h>

G_DECLARE_FINAL_TYPE(FlPluginRegistrarImpl,
                     fl_plugin_registrar_impl,
                     FL,
                     PLUGIN_REGISTRAR_IMPL,
                     GObject)

struct _FlPluginRegistrarImpl {
  GObject parent_instance;

  // View that plugin is controlling.
  FlView* view;

  // Messenger to communicate on.
  FlBinaryMessenger* messenger;

  // Texture registrar in use.
  FlTextureRegistrar* texture_registrar;
};

// Added here to stop the compiler from optimizing this function away.
G_MODULE_EXPORT GType fl_plugin_registrar_get_type();

static void fl_plugin_registrar_impl_iface_init(
    FlPluginRegistrarInterface* iface);

G_DEFINE_INTERFACE(FlPluginRegistrar, fl_plugin_registrar, G_TYPE_OBJECT)

G_DEFINE_TYPE_WITH_CODE(
    FlPluginRegistrarImpl,
    fl_plugin_registrar_impl,
    G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE(fl_plugin_registrar_get_type(),
                          fl_plugin_registrar_impl_iface_init))

static void fl_plugin_registrar_default_init(
    FlPluginRegistrarInterface* iface) {}

static void fl_plugin_registrar_impl_dispose(GObject* object) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(object);

  if (self->view != nullptr) {
    g_object_remove_weak_pointer(G_OBJECT(self->view),
                                 reinterpret_cast<gpointer*>(&(self->view)));
    self->view = nullptr;
  }
  g_clear_object(&self->messenger);
  g_clear_object(&self->texture_registrar);

  G_OBJECT_CLASS(fl_plugin_registrar_impl_parent_class)->dispose(object);
}

static void fl_plugin_registrar_impl_class_init(
    FlPluginRegistrarImplClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_plugin_registrar_impl_dispose;
}

static FlBinaryMessenger* get_messenger(FlPluginRegistrar* registrar) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(registrar);
  return self->messenger;
}

static FlTextureRegistrar* get_texture_registrar(FlPluginRegistrar* registrar) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(registrar);
  return self->texture_registrar;
}

static FlView* get_view(FlPluginRegistrar* registrar) {
  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(registrar);
  return self->view;
}

static void fl_plugin_registrar_impl_iface_init(
    FlPluginRegistrarInterface* iface) {
  iface->get_messenger = get_messenger;
  iface->get_texture_registrar = get_texture_registrar;
  iface->get_view = get_view;
}

static void fl_plugin_registrar_impl_init(FlPluginRegistrarImpl* self) {}

FlPluginRegistrar* fl_plugin_registrar_new(
    FlView* view,
    FlBinaryMessenger* messenger,
    FlTextureRegistrar* texture_registrar) {
  g_return_val_if_fail(view == nullptr || FL_IS_VIEW(view), nullptr);
  g_return_val_if_fail(FL_IS_BINARY_MESSENGER(messenger), nullptr);
  g_return_val_if_fail(FL_IS_TEXTURE_REGISTRAR(texture_registrar), nullptr);

  FlPluginRegistrarImpl* self = FL_PLUGIN_REGISTRAR_IMPL(
      g_object_new(fl_plugin_registrar_impl_get_type(), nullptr));

  // Added to stop compiler complaining about an unused function.
  FL_IS_PLUGIN_REGISTRAR_IMPL(self);

  self->view = view;
  if (view != nullptr) {
    g_object_add_weak_pointer(G_OBJECT(view),
                              reinterpret_cast<gpointer*>(&(self->view)));
  }
  self->messenger = FL_BINARY_MESSENGER(g_object_ref(messenger));
  self->texture_registrar =
      FL_TEXTURE_REGISTRAR(g_object_ref(texture_registrar));

  return FL_PLUGIN_REGISTRAR(self);
}

G_MODULE_EXPORT FlBinaryMessenger* fl_plugin_registrar_get_messenger(
    FlPluginRegistrar* self) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRAR(self), nullptr);

  return FL_PLUGIN_REGISTRAR_GET_IFACE(self)->get_messenger(self);
}

G_MODULE_EXPORT FlTextureRegistrar* fl_plugin_registrar_get_texture_registrar(
    FlPluginRegistrar* self) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRAR(self), nullptr);

  return FL_PLUGIN_REGISTRAR_GET_IFACE(self)->get_texture_registrar(self);
}

G_MODULE_EXPORT FlView* fl_plugin_registrar_get_view(FlPluginRegistrar* self) {
  g_return_val_if_fail(FL_IS_PLUGIN_REGISTRAR(self), nullptr);

  return FL_PLUGIN_REGISTRAR_GET_IFACE(self)->get_view(self);
}

G_MODULE_EXPORT void fl_plugin_registrar_register_view_factory(
    FlPluginRegistrar* self,
    FlPlatformViewFactory* factory,
    const gchar* view_type) {
  g_return_if_fail(FL_IS_PLUGIN_REGISTRAR_IMPL(self));

  if (FL_PLUGIN_REGISTRAR_IMPL(self)->view) {
    FlPlatformViewsPlugin* plugin =
        fl_view_get_platform_views_plugin(FL_PLUGIN_REGISTRAR_IMPL(self)->view);
    fl_platform_views_plugin_register_view_factory(plugin, factory, view_type);
  } else {
    g_warning("Cannot register view factory before FlView is initialized");
  }
}
