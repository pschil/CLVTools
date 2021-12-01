#include "pnbd_dyncov_LL_new.h"

// arma::vec pnbd_dyncov_LL_i_cov_exp(arma::vec params, arma::vec cov){
//   return();
// }

// struct CBS {
//   CBS(const int x, const double t_x, const double T_cal):x(x), t_x(t_x), T_cal(T_cal){};
//   const int x;
//   const double t_x;
//   const double T_cal;
// };

struct Customer {
  const std::list<Walk> real_walks, aux_walks;

  const int x;
  const double t_x, T_cal;
  const double adj_transaction_cov_dyn, adj_lifetime_cov_dyn;

  Customer(const int x, const double t_x, const double T_cal,
           const double adj_transaction_cov_dyn, const double adj_lifetime_cov_dyn,
           const arma::vec& adj_cov_data_life, const arma::vec& adj_cov_data_trans,
           const arma::mat& walks_from_to);
};

Customer::Customer(const int x, const double t_x, const double T_cal,
                   const double adj_transaction_cov_dyn, const double adj_lifetime_cov_dyn,
                   const arma::vec& adj_cov_data_life, const arma::vec& adj_cov_data_trans,
                   const arma::mat& walks_from_to)
  : x(x), t_x(t_x), T_cal(T_cal),
    adj_transaction_cov_dyn(adj_transaction_cov_dyn), adj_lifetime_cov_dyn(adj_lifetime_cov_dyn){


}

Walk::Walk(const arma::vec& cov_data, const arma::uword from, const arma::uword to,
           const double tjk, const double d, const int delta)
  : walk_data(cov_data.subvec(from, to)), tjk(tjk), d(d), delta(delta) {
}

double Walk::sum_middle_elems() const{
  return(arma::accu(this->walk_data.subvec(1, this->walk_data.n_elem-2)));
}

arma::uword Walk::n_elem() const{
  return(this->walk_data.n_elem);
}

double Walk::first() const{
  return(this->walk_data.front());
}

double Walk::last() const{
  return(this->walk_data.back());
}


double pnbd_dyncov_LL_i_hyp_alpha_ge_beta(const double r, const double s,
                                          const int x,
                                          const double alpha_1, const double beta_1,
                                          const double alpha_2, const double beta_2){
  const double z1 = 1 - (beta_1/alpha_1);
  const double z2 = 1 - (beta_2/alpha_2);
  const double log_C = std::lgamma(r + s + x + 1) +
    std::lgamma(s) +
    std::lgamma(r + s + x) +
    std::lgamma(s + 1);

  gsl_sf_result gsl_res;
  int status;
  double hyp_z1;
  double hyp_z2;

  status = gsl_sf_hyperg_2F1_e(r + s + x,
                               s + 1,
                               r + s + x + 1,
                               z1,
                               &gsl_res);
  if(status == 11 || status == 1){
    hyp_z1 = std::pow(1 - z1, r + x) * std::exp(log_C) / std::pow(beta_1, r + s + x);
  }else{
    hyp_z1 = gsl_res.val / std::pow(alpha_1, r + s + x);
  }


  status = gsl_sf_hyperg_2F1_e(r + s + x,
                               s + 1,
                               r + s + x + 1,
                               z2,
                               &gsl_res);
  if(status == 11 || status == 1){
    hyp_z2 = std::pow(1 - z2, r + x) * std::exp(log_C) / std::pow(beta_2, r + s + x);
  }else{
    hyp_z2 = gsl_res.val / std::pow(alpha_2, r + s + x);
  }

  return(hyp_z1 - hyp_z2);
}

double pnbd_dyncov_LL_i_hyp_beta_g_alpha(const double r, const double s,
                                         const int x,
                                         const double alpha_1, const double beta_1,
                                         const double alpha_2, const double beta_2){

  const double z1 = 1 - (beta_1/alpha_1);
  const double z2 = 1 - (beta_2/alpha_2);
  const double log_C = std::lgamma(r + s + x + 1) +
    std::lgamma(s) +
    std::lgamma(r + s + x) +
    std::lgamma(s + 1);

    gsl_sf_result gsl_res;
    double hyp_z1;
    double hyp_z2;

    int status = gsl_sf_hyperg_2F1_e(r + s + x,
                                     s + 1,
                                     r + s + x + 1,
                                     z1, &gsl_res);
    if(status == 11 || status == 1){
      hyp_z1 = std::pow(1 - z1, r + x) * std::exp(log_C) / std::pow(beta_1, r + s + x);
    }else{
      hyp_z1 = gsl_res.val / std::pow(alpha_1, r + s + x);
    }


    status = gsl_sf_hyperg_2F1_e(r + s + x,
                                 s + 1,
                                 r + s + x + 1,
                                 z2, &gsl_res);

    if(status == 11 || status == 1){
      hyp_z2 = std::pow(1 - z2, r + x) * std::exp(log_C) / std::pow(beta_2, r + s + x);
    }else{
      hyp_z2 = gsl_res.val / std::pow(alpha_2, r + s + x);
    }

    return(hyp_z1 - hyp_z2);
}

double pnbd_dyncov_LL_i_bksumbjsum_walk_i(const Walk& w){
  if(w.n_elem() == 1){
    return(w.first()*w.d + w.last()*(w.tjk - w.d - w.delta*(w.n_elem() - 2)));
  }else{
    if(w.n_elem() == 2){
      return(w.first()*w.d + w.last());
    }else{
      // > {1,2}
      return(w.first() + w.sum_middle_elems() + w.last());
    }
  }
}

double pnbd_dyncov_LL_i_BjSum(const std::list<Walk>& real_walks){
  double bjsum = 0;
  for(Walk w : real_walks){
    bjsum += pnbd_dyncov_LL_i_bksumbjsum_walk_i(w);
  }
  return(bjsum);
}

double pnbd_dyncov_LL_i_BkSum(const std::list<Walk>& real_walks, const std::list<Walk>& aux_walks){
  double bksum = pnbd_dyncov_LL_i_BjSum(real_walks);
  for(Walk w : aux_walks){
    bksum += pnbd_dyncov_LL_i_bksumbjsum_walk_i(w);
  }
  return(bksum);
}


double pnbd_dyncov_LL_i_F2_1(const double r, const double alpha_0, const double s, const double beta_0,
                             const int x, const double dT,
                             const double a1, const double b1,
                             const double A1T, const double C1T){

  const double alpha_1 = a1 + (1-dT)*A1T + alpha_0;
  const double beta_1 = (b1 + (1-dT)*C1T + beta_0) * A1T/C1T;

  const double alpha_2 = a1 + A1T + alpha_0;
  const double beta_2  = (b1 + C1T + beta_0)*A1T/C1T;

  double F2_1 = 0;
  if( alpha_1 >= beta_1){
    F2_1 = std::pow(A1T/C1T, s) * pnbd_dyncov_LL_i_hyp_alpha_ge_beta(r, s, x,
                    alpha_1, beta_1,
                    alpha_2, beta_2);

  }else{
    F2_1 = std::pow(A1T/C1T, s) * pnbd_dyncov_LL_i_hyp_beta_g_alpha(r, s, x,
                    alpha_1, beta_1,
                    alpha_2, beta_2);
  }
  return(F2_1);
}

double pnbd_dyncov_LL_i_F2_2(const double r, const double alpha_0, const double s, const double beta_0,
                             const int x,
                             const double akt, const double bkT,
                             const double aT, const double bT,
                             const double AkT, const double CkT){

  const double alpha_1 = akt + alpha_0;
  const double beta_1 =  (bkT + beta_0)*AkT/CkT;

  const double alpha_2 = (aT + alpha_0);
  const double beta_2 =  (bT + beta_0)*AkT/CkT;

  double F2_2 = 0;
  if(alpha_1 >= beta_1){
    F2_2 = std::pow(AkT/CkT, s) * pnbd_dyncov_LL_i_hyp_alpha_ge_beta(r, s, x,
                    alpha_1, beta_1,
                    alpha_2, beta_2);
  }else{
    F2_2 = std::pow(AkT/CkT, s) * pnbd_dyncov_LL_i_hyp_beta_g_alpha(r, s, x,
                    alpha_1, beta_1,
                    alpha_2, beta_2);
  }
  return(F2_2);
}

// double pnbd_dyncov_LL_i_F2_3(){
//
// }

double pnbd_dyncov_LL_i_F2(const int num_walks,
                           const double r, const double alpha_0, const double s, const double beta_0,
                           const int x, const double t_x, const double T_cal, const double dT,
                           const double Bjsum, const double B1,
                           const double D1,
                           const double BT, const double DT,
                           const double A1T, const double C1T,
                           const double AkT, const double CkT,
                           const double F2_3){


  // const double Bjsum;

  const double a1T = Bjsum + B1 + T_cal * A1T;
  const double b1T = D1 + T_cal * C1T;
  const double a1 = Bjsum + B1 + A1T * (t_x + dT - 1);
  const double b1  = D1 + C1T * (t_x + dT - 1);

  // **TODO: What is num_walks here? Length of 1 walk or all walks, incl aux or only real walks?**
  if(num_walks == 1){

    const double alpha_1 = a1 + (1-dT)*A1T + alpha_0;
    const double beta_1  = (b1 + (1-dT)*C1T + beta_0) * A1T/C1T;

    const double alpha_2 = a1T + alpha_0;
    const double beta_2  = (b1T  + beta_0)*A1T/C1T;

    // num_walk==1 has no F2.2 and F2.3
    double F2_1;
    if(alpha_1 >= beta_1){
      F2_1 = std::pow(A1T/C1T, s) * pnbd_dyncov_LL_i_hyp_alpha_ge_beta(r, s, x,
                      alpha_1, beta_1,
                      alpha_2, beta_2);
    }else{
      F2_1 = std::pow(A1T/C1T, s) * pnbd_dyncov_LL_i_hyp_beta_g_alpha(r, s, x,
                      alpha_1, beta_1,
                      alpha_2, beta_2);
    }

    return(F2_1);

  }else{

    const double akt = Bjsum + BT + AkT * (t_x + dT + num_walks - 2);
    const double bkT = DT + CkT * (t_x + dT + num_walks - 2);
    const double aT = Bjsum + BT + (T_cal * AkT);
    const double bT  = DT + T_cal * CkT;

    const double F2_1 = pnbd_dyncov_LL_i_F2_1(r, alpha_0, s, beta_0,
                                              x, dT,
                                              a1, b1,
                                              A1T, C1T);
    const double F2_2 = pnbd_dyncov_LL_i_F2_2(r, alpha_0, s, beta_0,
                                              x,
                                              akt, bkT,
                                              aT, bT,
                                              AkT, CkT);
    // const double F2_3 = pnbd_dyncov_LL_i_F2_3();

    return(F2_1 + F2_2 + F2_3);
  }
}

// [[Rcpp::export]]
double pnbd_dyncov_LL_i(const double r, const double alpha_0, const double s, const double beta_0,
                        const int x,
                        const double t_x,
                        const double T_cal,
                        const int num_walks,
                        const double adj_transaction_cov_dyn,
                        const double adj_lifetime_cov_dyn,
                        const double dT,
                        const double A1T_R,
                        const double C1T_R,
                        const double A1sum_R,
                        const double Bjsum, const double Bksum, const double B1, const double BT,
                        const double DT, const double D1,
                        const double F2_3){

  // const arma::vec params_cov_life, params_cov_trans;
  // const arma::mat cov_data_life; //k x T matrix of covariates
  // const arma::mat cov_data_trans;
  //
  // const arma::vec adj_cov_data_life = arma::exp(params_cov_life.t() * cov_data_life);


  // // #data.work.trans[AuxTrans==T, adj.transaction.cov.dyn]]
  // const double adj_transaction_cov_dyn,
  // // #cbs[, CkT:= data.work.life[AuxTrans==T, adj.lifetime.cov.dyn]]
  // const double adj_lifetime_cov_dyn,
  // // #cbs[, dT:= data.work.trans[AuxTrans==T, d]]
  // const double dT,
  // // #cbs[, A1T:= data.work.trans[AuxTrans==T, adj.Walk1]]
  // // #const arma::vec& cov_aux_trans,
  // const double A1T_R,
  // // #cbs[, C1T:= data.work.life[AuxTrans==T, adj.Walk1]]
  // // #const arma::vec& cov_aux_life,
  // const double C1T_R,

  // Transaction or Purchase Process ---------------------------------------------------


  const double A1T = A1T_R; //cov_aux_trans(0);
  double A1sum;
  if(x == 0){
    A1sum = 0;
  }else{
    A1sum = A1sum_R;
  }

  const double AkT = adj_transaction_cov_dyn;

  // const double B1;
  // const double BT;

  // const double Bksum;


  // Lifetime Process ---------------------------------------------------

  const double C1T = C1T_R; //cov_aux_life(0);
  const double CkT = adj_lifetime_cov_dyn;

  // const double DT;
  // const double D1;

  const double DkT = CkT * T_cal + DT;

  const double log_F0 = r*std::log(alpha_0) + s*std::log(beta_0) + lgamma(x+r) - lgamma(r) + A1sum;
  const double log_F1 = std::log(s) - std::log(r+s+x);

  const double F2 = pnbd_dyncov_LL_i_F2(num_walks,
                                        r, alpha_0,  s,  beta_0,
                                        x,  t_x,  T_cal,  dT,
                                        Bjsum,  B1,
                                        D1, BT,  DT,
                                        A1T,  C1T, AkT,  CkT,
                                        F2_3);

  const double log_F3 = -s * std::log(DkT + beta_0) - (x+r) * std::log(Bksum + alpha_0);

  // *** TODO: == 0?? 1e-16 xxx?
  double LL = 0;
  if(F2 < 0){
    LL = log_F0 + log_F3 + std::log1p(std::exp(log_F1-log_F3) * F2);
  }else{
    if(F2>0){
      double max_AB = std::fmax(log_F1 + std::log(F2), log_F3);
      LL = log_F0 + max_AB  + std::log(std::exp(log_F1 + std::log(F2) - max_AB) + std::exp(log_F3 - max_AB));
    }else{
      LL = log_F0 + log_F3;
    }
  }

  return(LL);
}


// arma::vec pnbd_dyncov_LL_new_ind(
//     const double r,
//     const double alpha_0,
//     const double s,
//     const double beta_0,
//     const arma::vec& vX,
//     const arma::vec& vT_x,
//     const arma::vec& vT_cal,
//     const arma::vec& vNumWalks,
//     const arma::vec& vAdjTransactionCovDyn,
//     const arma::vec& vAdjLifetimeCovDyn,
//     const arma::vec& vdT,
//     const arma::vec& vA1sum_R,
//     const arma::vec& vBjsum,
//     const arma::vec& vBksum,
//     const arma::vec& vB1,
//     const arma::vec& vBT,
//     const arma::vec& vDT,
//     const arma::vec& vD1,
//     const arma::vec& vF2_3,
//     const arma::mat& mCov_aux
//     ){
//
//   arma::uword n = vX.n_elem;
//   arma::vec vRes = arma::zeros(n);
//   for(arma::uword i = 0; i < n; i++){
//     vRes(i) = pnbd_dyncov_LL_i( r,  alpha_0,  s,  beta_0,
//          vX(i), vT_x(i), vT_cal(i),
//          vNumWalks(i),
//          vAdjTransactionCovDyn(i),
//          vAdjLifetimeCovDyn(i),
//          vdT(i),
//          mCov_aux.col(i),
//          vA1sum_R(i),
//          vBjsum(i),  vBksum(i),  vB1(i),  vBT(i),
//          vDT(i),  vD1(i),
//          vF2_3(i));
//   }
//
//
//   return(vRes);
// }

