<?php

function opigno_collaborative_workspaces_form_opigno_moxtra_app_moxtra_configuration_form_alter(&$form, &$form_state, $form_id)
{
  $org_id = variable_get('opigno_moxtra_app_org_id');
  if (!empty($org_id)) {

    $info = opigno_moxtra_app_api_opigno_get_max_users_organization();

    if (isset($info->max_total_users))
      if ($info->max_total_users == 0)
        $form['coll_wrkspce_info'] = array(
          '#type' => 'markup',
          '#markup' => '<p>'. t('You are using the free version of Collaborative Workspaces.') .' '.
            l(t('Upgrade !'), OPIGNO_MOXTRA_APP_OPIGNO_RENEW_GATEWAY. $org_id, array('attributes' => array('target' => '_blank'))) .
            '</p>'
        );
      else
        $form['coll_wrkspce_info_max_users'] = array(
          '#type' => 'markup',
          '#markup' => '<p>'.
            t('You have created @current / @max users.',
              array('@current' => $info->current_total_users, '@max' => $info->max_total_users)) .' '.
            l(t('Want more ?'), OPIGNO_MOXTRA_APP_OPIGNO_RENEW_GATEWAY. $org_id, array('attributes' => array('target' => '_blank'))).
            '</p>',
        );
    else
      $form['coll_wrkspce_error'] = array(
        '#type' => 'markup',
        '#markup' => '<p>'. t('Error while catching the max users information') .'</p>'
      );
  }

  return $form;
}