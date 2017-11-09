<?php

/**
 * Form constructor for the moxtra configuration form.
 *
 * @param array $form
 * @param array $form_state
 *
 * @ingroup forms
 *
 * @return array
 */
function opigno_moxtra_app_moxtra_configuration_form($form, &$form_state)
{
  $org_id = variable_get('opigno_moxtra_app_org_id');
  if (empty($org_id))
    $form['button_create_form'] = array(
      '#type' => 'submit',
      '#submit' => array('opigno_moxtra_app_moxtra_configuration_form_submit'),
      '#value' => t('Activate the app on Opigno.org'),
      '#name' => 'create_organization'
    );

  else {
    $form['org_id'] = array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('Your organization ID on Opigno.org is') . ': ' . $org_id . '</p>'
    );

    if (!module_exists('opigno_collaborative_workspaces')) {
      $form['install_collaborative_workspace'] = array(
        '#type' => 'submit',
        '#name' => 'install_collaborative_workspace',
        '#value' => t('Install the collaborative workspace module')
      );
    }

    if (!module_exists('opigno_live_meetings')) {
      $form['install_live_meetings'] = array(
        '#type' => 'submit',
        '#name' => 'install_live_meetings',
        '#value' => t('Install the live meetings module')
      );
    }
  }

  return $form;
}

/**
 * Form submit handler for the moxtra configuration form.
 *
 * @param array $form
 * @param array $form_state
 *
 * @ingroup forms
 */
function opigno_moxtra_app_moxtra_configuration_form_submit($form, &$form_state) {
  if ($form_state['triggering_element']['#name'] == 'create_organization') {
    $response = opigno_moxtra_app_api_opigno_create_organization();
    if (isset($response->opigno_error_message))
      drupal_set_message(t('An error occurred while creating the organization. Try again, check the logs or contact an opigno.org administrator.'));
    else {
      drupal_set_message(t('Organization created successfully. You can now use the Moxtra App.'));

      // Install automatically the collaborative workspace as well.
      module_enable(array('opigno_collaborative_workspaces'));
      drupal_flush_all_caches();
    }
  }

  if ($form_state['triggering_element']['#name'] == 'install_collaborative_workspace') {
    module_enable(array('opigno_collaborative_workspaces'));
    drupal_flush_all_caches();
  }

  if ($form_state['triggering_element']['#name'] == 'install_live_meetings') {
    module_enable(array('opigno_live_meetings'));
    drupal_flush_all_caches();
  }
}